#include <string>
#include <sstream>
#include <http-parser/http_parser.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <tubekit-log/logger.h>

#include "task/http_task.h"
#include "socket/socket_handler.h"
#include "utility/singleton.h"
#include "connection/http_connection.h"
#include "app/http_app.h"
#include "connection/connection_mgr.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::connection;
using namespace tubekit::app;

http_parser_settings *http_task::settings = nullptr;

http_task::http_task(tubekit::socket::socket *m_socket) : task(m_socket),
                                                          reason_recv(false),
                                                          reason_send(false)
{
    if (settings == nullptr)
    {
        settings = new http_parser_settings;
        settings->on_message_begin = [](http_parser *parser) -> auto
        {
            connection::http_connection *t_http_connection = static_cast<connection::http_connection *>(parser->data);
            http_method method = (http_method)parser->method;
            t_http_connection->method = http_method_str(method);
            return 0;
        };

        settings->on_url = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::http_connection *t_http_connection = static_cast<connection::http_connection *>(parser->data);
            t_http_connection->url = std::string(at, length);
            return 0; // allowed
            // return -1;// reject this connection
        };

        settings->on_status = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            // parser reponses
            return 0;
        };

        settings->on_header_field = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::http_connection *t_http_connection = static_cast<connection::http_connection *>(parser->data);
            t_http_connection->head_field_tmp = std::string(at, length);
            return 0;
        };

        settings->on_header_value = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::http_connection *t_http_connection = static_cast<connection::http_connection *>(parser->data);
            std::string value(at, length);
            t_http_connection->add_header(t_http_connection->head_field_tmp, value);
            return 0;
        };

        settings->on_headers_complete = [](http_parser *parser) -> auto
        {
            return 0;
        };

        settings->on_body = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::http_connection *t_http_connection = static_cast<connection::http_connection *>(parser->data);
            t_http_connection->add_to_body(at, length);
            return 0;
        };

        settings->on_message_complete = [](http_parser *parser) -> auto
        {
            connection::http_connection *t_http_connection = static_cast<connection::http_connection *>(parser->data);
            t_http_connection->set_recv_end(true);
            return 0;
        };

        settings->on_chunk_header = [](http_parser *parser) -> auto
        {
            // std::cout << "on_chunk_header" << std::endl;
            return 0;
        };

        settings->on_chunk_complete = [](http_parser *parser) -> auto
        {
            // connection::http_connection *t_http_connection = static_cast<connection::http_connection *>(parser->data);
            // std::cout << "on_chunk_complete" << std::endl;
            return 0;
        };
    }
}

http_task::~http_task()
{
    destroy();
}

void http_task::destroy()
{
}

void http_task::run()
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
    connection::http_connection *t_http_connection = (connection::http_connection *)singleton<connection_mgr>::instance()->get(socket_ptr);

    // connection is close
    if (nullptr == t_http_connection || t_http_connection->is_close())
    {
        if (t_http_connection)
        {
            singleton<connection_mgr>::instance()->remove(socket_ptr);
        }

        // socket_handler next loop, executing remove(socket_ptr)
        singleton<socket_handler>::instance()->attach(socket_ptr, true);
        return;
    }

    // read from socket
    if (!t_http_connection->get_recv_end())
    {
        t_http_connection->buffer_used_len = 0;

        while (true)
        {
            int oper_errno = 0;
            t_http_connection->buffer_used_len = socket_ptr->recv(t_http_connection->buffer, t_http_connection->buffer_size, oper_errno);
            if (t_http_connection->buffer_used_len == -1 && oper_errno == EAGAIN)
            {
                t_http_connection->buffer_used_len = 0;
                break;
            }
            else if (t_http_connection->buffer_used_len == -1 && oper_errno == EINTR) // error interupt
            {
                t_http_connection->buffer_used_len = 0;
                continue;
            }
            else if (t_http_connection->buffer_used_len > 0)
            {
                int nparsed = http_parser_execute(t_http_connection->get_parser(), settings, t_http_connection->buffer, t_http_connection->buffer_used_len);
                if (t_http_connection->get_parser()->upgrade)
                {
                }
                else if (nparsed != t_http_connection->buffer_used_len) // error
                {
                    t_http_connection->buffer_used_len = 0;
                    t_http_connection->set_everything_end(true);
                    break;
                }
            }
            else
            {
                // 1. t_http_connection->buffer_used_len == 0 client closed
                t_http_connection->set_everything_end(true);
                break;
            }
        } // while(1)
    }

    // process http connection
    if (!t_http_connection->get_everything_end() && t_http_connection->get_recv_end() && !t_http_connection->get_process_end())
    {
        t_http_connection->set_process_end(true);
        // app loader,loading process_callback for t_http_connection
        app::http_app::process_connection(*t_http_connection);
        if (t_http_connection->process_callback)
        {
            t_http_connection->process_callback(*t_http_connection);
        }
        else
        {
            t_http_connection->set_everything_end(true);
        }
    }

    // write
    if (!t_http_connection->get_everything_end() && t_http_connection->get_process_end())
    {
        while (t_http_connection->buffer_used_len > t_http_connection->buffer_start_use)
        {
            int oper_errno = 0;
            int sended = socket_ptr->send(t_http_connection->buffer + t_http_connection->buffer_start_use, t_http_connection->buffer_used_len - t_http_connection->buffer_start_use, oper_errno);
            if (0 > sended)
            {
                if (oper_errno == EINTR)
                {
                    continue;
                }
                else if (oper_errno == EAGAIN)
                {
                    break;
                }
                else // error
                {
                    // 1.errno==ECONNRESET、EPIPE etc.
                    t_http_connection->set_response_end(true);
                    t_http_connection->set_everything_end(true);
                    break;
                }
            }
            else if (0 == sended) // disconnect or nothing to send
            {
                t_http_connection->set_everything_end(true);
                break;
            }
            else // send success
            {
                t_http_connection->buffer_start_use += sended;
            }
        }
        //  Notify the user that the content sent last time has been sent to the client
        if (t_http_connection->buffer_start_use == t_http_connection->buffer_used_len && 0 == t_http_connection->m_buffer.can_readable_size() && !t_http_connection->get_response_end())
        {
            if (t_http_connection->write_end_callback)
            {
                t_http_connection->write_end_callback(*t_http_connection);
            }
            else
            {
                t_http_connection->set_everything_end(true);
            }
        }
        if (t_http_connection->buffer_start_use == t_http_connection->buffer_used_len && 0 != t_http_connection->m_buffer.can_readable_size())
        {
            // read from m_buffer to buffer for next write
            try
            {
                t_http_connection->buffer_used_len = t_http_connection->m_buffer.read(t_http_connection->buffer, t_http_connection->buffer_size - 1);
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR(e.what());
            }
            t_http_connection->buffer[t_http_connection->buffer_used_len] = 0;
            t_http_connection->buffer_start_use = 0;
        }
        if (t_http_connection->buffer_start_use == t_http_connection->buffer_used_len && 0 == t_http_connection->m_buffer.can_readable_size() && t_http_connection->get_response_end())
        {
            t_http_connection->set_everything_end(true);
        }
    }

    // continue to epoll_wait
    if (!t_http_connection->get_everything_end() && !t_http_connection->get_recv_end()) // next loop for reading
    {
        singleton<socket_handler>::instance()->attach(socket_ptr); // continue registe epoll wait read
        return;
    }

    if (!t_http_connection->get_everything_end())
    {
        singleton<socket_handler>::instance()->attach(socket_ptr, true); // wait write
        return;
    }

    t_http_connection->set_everything_end(true);
    t_http_connection->mark_close();
    singleton<socket_handler>::instance()->attach(socket_ptr, true);
}
