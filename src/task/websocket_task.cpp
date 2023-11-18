#include "task/websocket_task.h"

#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include "connection/connection.h"
#include "socket/socket_handler.h"
#include <iostream>

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::connection;

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
            singleton<connection_mgr>::instance()->remove(socket_ptr);
        }
        singleton<socket_handler>::instance()->remove(socket_ptr);
        return;
    }

    if (t_websocket_connection->get_connected() == false)
    {
        if (t_websocket_connection->http_processed)
        {
            t_websocket_connection->mark_close();
            singleton<socket_handler>::instance()->attach(socket_ptr, true);
            return;
        }

        t_websocket_connection->buffer_used_len = 0;
        while (true)
        {
            t_websocket_connection->buffer_used_len = socket_ptr->recv(t_websocket_connection->buffer, t_websocket_connection->buffer_size);
            if (t_websocket_connection->buffer_used_len == -1 && errno == EAGAIN)
            {
                t_websocket_connection->buffer_start_use = 0;
                break;
            }
            else if (t_websocket_connection->buffer_used_len == -1 && errno == EINTR)
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
        t_websocket_connection->mark_close();
        singleton<socket_handler>::instance()->attach(socket_ptr, true);
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
            std::cout << "Sec-WebSocket-Key:" << t_websocket_connection->sec_websocket_key << "\n";
            std::cout << "Sec-WebSocket-Version:" << t_websocket_connection->sec_websocket_version << "\n";
            std::cout << "websocket http header parser succ, try send http header to client" << std::endl;
            t_websocket_connection->set_connected(true);
        }
    }

    if (t_websocket_connection->get_connected())
    {
        // TODO:read data from sock
        t_websocket_connection->everything_end = true;
    }

    // TODO:try send data

    if (t_websocket_connection->everything_end)
    {
        t_websocket_connection->mark_close();
        singleton<socket_handler>::instance()->attach(socket_ptr, true);
        return;
    }

    std::cout << "maybe something be done" << std::endl;
    singleton<socket_handler>::instance()->attach(socket_ptr);
    return;
}
