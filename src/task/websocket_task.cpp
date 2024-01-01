#include "task/websocket_task.h"

#include <tubekit-log/logger.h>

#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include "connection/connection.h"
#include "socket/socket_handler.h"
#include "app/websocket_app.h"
#include "utility/sha1.h"
#include "utility/base64.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::connection;
using namespace tubekit::app;

http_parser_settings *websocket_task::settings = nullptr;

websocket_task::websocket_task(tubekit::socket::socket *m_socket) : task(m_socket)
{
    if (settings == nullptr)
    {
        settings = new http_parser_settings;
        settings->on_message_begin = [](http_parser *parser) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            http_method method = (http_method)parser->method;
            t_websocket_connection->method = http_method_str(method);
            if (t_websocket_connection->method != "GET")
            {
                return -1;
            }
            return 0;
        };

        settings->on_url = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            t_websocket_connection->url = std::string(at, length);
            return 0;
        };

        settings->on_status = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            return 0;
        };

        settings->on_header_field = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            t_websocket_connection->head_filed_tmp = std::string(at, length);
            return 0;
        };

        settings->on_header_value = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            std::string value(at, length);
            t_websocket_connection->add_header(t_websocket_connection->head_filed_tmp, value);
            return 0;
        };

        settings->on_headers_complete = [](http_parser *parser) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            t_websocket_connection->http_processed = true;
            return 0;
        };

        settings->on_body = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            return 0;
        };

        settings->on_message_complete = [](http_parser *parser) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            t_websocket_connection->http_processed = true;
            return 0;
        };

        settings->on_chunk_header = [](http_parser *parser) -> auto
        {
            return -1;
        };

        settings->on_chunk_complete = [](http_parser *parser) -> auto
        {
            return -1;
        };
    }
}

websocket_task::~websocket_task()
{
    destroy();
}

void websocket_task::destroy()
{
}

void websocket_task::run()
{
    if (nullptr == m_data)
    {
        return;
    }
    socket::socket *socket_ptr = static_cast<socket::socket *>(m_data);

    if (!socket_ptr->close_callback)
    {
        socket_ptr->close_callback = [socket_ptr]() {
        };
    }

    // get connection layer instance
    connection::websocket_connection *t_websocket_connection = (connection::websocket_connection *)singleton<connection_mgr>::instance()->get(socket_ptr);

    if (nullptr == t_websocket_connection || t_websocket_connection->is_close())
    {
        if (t_websocket_connection)
        {
            if (t_websocket_connection->destory_callback)
            {
                t_websocket_connection->destory_callback(*t_websocket_connection);
            }
            singleton<connection_mgr>::instance()->remove(socket_ptr);
        }

        // socket_handler next loop, executing remove(socket_ptr)
        singleton<socket_handler>::instance()->attach(socket_ptr, true);
        return;
    }

    if (t_websocket_connection->get_connected() == false)
    {
        if (t_websocket_connection->http_processed)
        {
            singleton<connection_mgr>::instance()->mark_close(socket_ptr);
            return;
        }

        t_websocket_connection->buffer_used_len = 0;
        while (true)
        {
            int oper_errno = 0;
            t_websocket_connection->buffer_used_len = socket_ptr->recv(t_websocket_connection->buffer, t_websocket_connection->buffer_size, oper_errno);
            if (t_websocket_connection->buffer_used_len == -1 && oper_errno == EAGAIN)
            {
                t_websocket_connection->buffer_start_use = 0;
                break;
            }
            else if (t_websocket_connection->buffer_used_len == -1 && oper_errno == EINTR)
            {
                t_websocket_connection->buffer_used_len = 0;
                continue;
            }
            else if (t_websocket_connection->buffer_used_len > 0)
            {
                int nparsed = http_parser_execute(t_websocket_connection->get_parser(),
                                                  settings,
                                                  t_websocket_connection->buffer,
                                                  t_websocket_connection->buffer_used_len);
                if (t_websocket_connection->get_parser()->upgrade)
                {
                    t_websocket_connection->is_upgrade = true;
                }
                else if (nparsed != t_websocket_connection->buffer_used_len)
                {
                    t_websocket_connection->buffer_used_len = 0;
                    t_websocket_connection->everything_end = true;
                    t_websocket_connection->http_processed = true;
                    break;
                }
            }
            else
            {
                t_websocket_connection->http_processed = true;
                t_websocket_connection->everything_end = true;
                break;
            }
            t_websocket_connection->buffer_used_len = 0;
        } // while(1)
    }

    if (t_websocket_connection->http_processed == false && t_websocket_connection->everything_end == false)
    {
        singleton<socket_handler>::instance()->attach(socket_ptr);
        return;
    }

    // is not websocket
    if (!(t_websocket_connection->http_processed && t_websocket_connection->is_upgrade))
    {
        singleton<connection_mgr>::instance()->mark_close(socket_ptr);
        return;
    }

    // process connect protocol
    if (t_websocket_connection->get_connected() == false && t_websocket_connection->http_processed && t_websocket_connection->everything_end == false)
    {
        for (auto &header : t_websocket_connection->headers)
        {
            if (header.first == "Sec-WebSocket-Key" && header.second.size() >= 1)
            {
                t_websocket_connection->sec_websocket_key = header.second[0];
            }
            if (header.first == "Sec-WebSocket-Version" && header.second.size() >= 1)
            {
                t_websocket_connection->sec_websocket_version = header.second[0];
            }
        }
        if (t_websocket_connection->sec_websocket_key.empty() || t_websocket_connection->sec_websocket_version != "13")
        {
            t_websocket_connection->everything_end = true;
        }
        else
        {
            // response websocket connect header
            std::string combine_key = t_websocket_connection->sec_websocket_key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

            // calculate SHA1 hash
            utility::SHA1 sha1;
            sha1.update(combine_key);
            std::string sha1_str = sha1.final();

            // Base64 encode the hash
            std::string base64_encoded = base64::base64_encode(sha1.to_binary(sha1_str));
            std::string response = "HTTP/1.1 101 Switching Protocols\r\n";
            response += "Upgrade: websocket\r\n";
            response += "Connection: Upgrade\r\n";
            response += "Sec-WebSocket-Accept: " + base64_encoded + "\r\n\r\n";

            t_websocket_connection->send(response.c_str(), response.size());
            t_websocket_connection->set_connected(true);
        }
    }

    if (t_websocket_connection->get_connected())
    {
        // read data from socket to connection layer buffer
        if (false == t_websocket_connection->sock2buf())
        {
            singleton<connection_mgr>::instance()->mark_close(socket_ptr);
            return;
        }
        // process data
        {
            websocket_app::process_connection(*t_websocket_connection);
        }
        // send data
        bool b_send = false;
        {
            // send data to socket from connection layer
            b_send = t_websocket_connection->buf2sock();
        }
        if (b_send)
        {
            int i_ret = singleton<socket_handler>::instance()->attach(socket_ptr, b_send);
            if (i_ret != 0)
            {
                LOG_ERROR("socket handler attach error %d", i_ret);
            }
            return;
        }
    }

    if (t_websocket_connection->everything_end)
    {
        singleton<connection_mgr>::instance()->mark_close(socket_ptr);
        return;
    }

    singleton<socket_handler>::instance()->attach(socket_ptr);
    return;
}
