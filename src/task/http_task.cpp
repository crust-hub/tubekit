#include <string>
#include <sstream>
#include <http-parser/http_parser.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "task/http_task.h"
#include "socket/socket_handler.h"
#include "utility/singleton_template.h"
#include "request/http_request.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::request;

http_parser_settings *http_task::settings = nullptr;

http_task::http_task(tubekit::socket::socket *m_socket) : task(m_socket)
{
    if (settings == nullptr)
    {
        settings = new http_parser_settings;
        settings->on_message_begin = [](http_parser *parser) -> auto
        {
            request::http_request *m_http_request = static_cast<request::http_request *>(parser->data);
            http_method method = (http_method)parser->method;
            m_http_request->method = http_method_str(method);
            return 0;
        };

        settings->on_url = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            request::http_request *m_http_request = static_cast<request::http_request *>(parser->data);
            m_http_request->url = std::string(at, length);
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
            request::http_request *m_http_request = static_cast<request::http_request *>(parser->data);
            m_http_request->head_field_tmp = std::string(at, length);
            return 0;
        };

        settings->on_header_value = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            request::http_request *m_http_request = static_cast<request::http_request *>(parser->data);
            std::string value(at, length);
            m_http_request->add_header(m_http_request->head_field_tmp, value);
            return 0;
        };

        settings->on_headers_complete = [](http_parser *parser) -> auto
        {
            return 0;
        };

        settings->on_body = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            request::http_request *m_http_request = static_cast<request::http_request *>(parser->data);
            m_http_request->add_to_body(at, length);
            return 0;
        };

        settings->on_message_complete = [](http_parser *parser) -> auto
        {
            request::http_request *m_http_request = static_cast<request::http_request *>(parser->data);
            m_http_request->set_recv_end(true);
            return 0;
        };

        settings->on_chunk_header = [](http_parser *parser) -> auto
        {
            std::cout << "on_chunk_header" << std::endl;
            return 0;
        };

        settings->on_chunk_complete = [](http_parser *parser) -> auto
        {
            // request::http_request *m_http_request = static_cast<request::http_request *>(parser->data);
            std::cout << "on_chunk_complete" << std::endl;
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
    socket_handler *handler = singleton_template<socket_handler>::instance();
    socket::socket *socketfd = static_cast<socket::socket *>(m_data);

    if (!socketfd->delete_ptr_hook)
    {
        // execute delete_ptr_hook when socket to close
        socketfd->delete_ptr_hook = [](void *ptr) -> void
        {
            if (ptr != nullptr)
            {
                request::http_request *m_http_request = (request::http_request *)ptr;
                delete m_http_request;
            }
        };
    }

    if (socketfd->ptr == nullptr) // binding httpRequest for socket
    {
        request::http_request *m_http_request = new request::http_request(socketfd->get_fd());
        socketfd->ptr = (void *)m_http_request;
    }

    request::http_request *m_http_request = static_cast<request::http_request *>(socketfd->ptr);

    // read from socket
    if (!m_http_request->get_recv_end())
    {
        m_http_request->buffer_used_len = 0;
        while (true)
        {
            m_http_request->buffer_used_len = socketfd->recv(m_http_request->buffer, m_http_request->buffer_size);
            if (m_http_request->buffer_used_len == -1 && errno == EAGAIN)
            {
                m_http_request->buffer_used_len = 0;
                break;
            }
            else if (m_http_request->buffer_used_len == -1 && errno == EINTR) // error interupt
            {
                m_http_request->buffer_used_len = 0;
                continue;
            }
            else if (m_http_request->buffer_used_len == 0) // noting recv later
            {
                m_http_request->buffer_used_len = 0;
                break;
            }
            else if (m_http_request->buffer_used_len > 0)
            {
                int nparsed = http_parser_execute(m_http_request->get_parser(), settings, m_http_request->buffer, m_http_request->buffer_used_len);
                if (m_http_request->get_parser()->upgrade)
                {
                }
                else if (nparsed != m_http_request->buffer_used_len) // error
                {
                    m_http_request->buffer_used_len = 0;
                    break;
                }
            }
            m_http_request->buffer_used_len = 0;
        } // while(1)
    }

    // process http request
    if (m_http_request->get_recv_end() && !m_http_request->get_process_end())
    {
        m_http_request->set_process_end(true);
        // load callback
        m_http_request->process_callback = [](http_request &request) -> void
        {
            const char *response = "HTTP/1.1 200 OK\r\nServer: tubekit\r\nContent-Type: text/json;\r\n\r\n{\"server\":\"tubekit\"}";
            request.m_buffer.write(response, strlen(response));
            // Write when the contents of the buffer have been sent write_end_callback will be executed,
            // and the response must be set response_end to true, then write after write_end_callback will be continuously recalled
            request.write_end_callback = [](http_request &m_request) -> void
            {
                // std::cout << "m_buffer send over" << std::endl;
                m_request.set_response_end(true);
            };
            request.destory_callback = [](http_request &m_request) -> void
            {
                // std::cout << "http_request destory" << std::endl;
            };
        };
        if (m_http_request->process_callback)
        {
            m_http_request->process_callback(*m_http_request);
        }
        else
        {
            m_http_request->set_everything_end(true);
        }
    }

    // write
    if (m_http_request->get_process_end() && !m_http_request->get_everything_end())
    {
        while (m_http_request->buffer_used_len > m_http_request->buffer_start_use)
        {
            int sended = socketfd->send(m_http_request->buffer + m_http_request->buffer_start_use, m_http_request->buffer_used_len - m_http_request->buffer_start_use);
            if (0 > sended)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                else if (errno == EAGAIN)
                {
                    break;
                }
                else // error
                {
                    m_http_request->set_everything_end(true);
                    break;
                }
            }
            else if (0 == sended) // disconnect or nothing to send
            {
                m_http_request->set_everything_end(true);
                break;
            }
            else // send success
            {
                m_http_request->buffer_start_use += sended;
            }
        }
        //  Notify the user that the content sent last time has been sent to the client
        if (m_http_request->buffer_start_use == m_http_request->buffer_used_len && 0 == m_http_request->m_buffer.can_readable_size() && !m_http_request->get_response_end())
        {
            if (m_http_request->write_end_callback)
            {
                m_http_request->write_end_callback(*m_http_request);
            }
            else
            {
                m_http_request->set_everything_end(true);
            }
        }
        if (m_http_request->buffer_start_use == m_http_request->buffer_used_len && 0 != m_http_request->m_buffer.can_readable_size())
        {
            // read from m_buffer to buffer for next write
            m_http_request->buffer_used_len = m_http_request->m_buffer.read(m_http_request->buffer, m_http_request->buffer_size - 1);
            m_http_request->buffer[m_http_request->buffer_used_len] = 0;
            m_http_request->buffer_start_use = 0;
        }
        if (m_http_request->buffer_start_use == m_http_request->buffer_used_len && 0 == m_http_request->m_buffer.can_readable_size() && m_http_request->get_response_end())
        {
            m_http_request->set_everything_end(true);
        }
    }

    // continue to epoll_wait
    if (!m_http_request->get_recv_end()) // next loop for reading
    {
        handler->attach(socketfd); // continue registe epoll wait read
    }
    else if (!m_http_request->get_everything_end())
    {
        handler->attach(socketfd, true); // wait write
    }
    else
    {
        handler->remove(socketfd); // remove
    }
}
