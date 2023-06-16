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
            // session::http_session *m_session = static_cast<session::http_session *>(parser->data);
            // std::cout << "on_headers_complete" << std::endl;
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
            m_http_request->set_recv_over(true);
            // std::cout << "on_message_complete" << std::endl;
            return 0;
        };

        settings->on_chunk_header = [](http_parser *parser) -> auto
        {
            // request::http_request *m_http_request = static_cast<request::http_request *>(parser->data);
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

    // read
    if (false == m_http_request->get_recv_over())
    {
        m_http_request->buffer_used_len = 0;
        while (1)
        {
            m_http_request->buffer_used_len = socketfd->recv(m_http_request->buffer, m_http_request->buffer_size);
            if (m_http_request->buffer_used_len == -1 && errno == EAGAIN)
            {
                // std::cout << "EAGAIN :" << strerror(errno) << std::endl;
                break;
            }
            else if (m_http_request->buffer_used_len == -1 && errno == EINTR) // error interupt
            {
                continue;
            }
            else if (m_http_request->buffer_used_len == 0) // noting recv later
            {
                // std::cout << "m_http_request->buffer_used_len==0" << std::endl;
                m_http_request->set_recv_over(true);
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
                    break;
                }
            }
        } // while(1)
    }

    // process http request
    if (m_http_request->get_recv_over() && false == m_http_request->get_processed() && false == m_http_request->get_send_over())
    {
        // test(m_http_request);
        const char *response = "HTTP/1.1 200 OK\r\nServer: tubekit\r\nContent-Type: text/json;\r\n\r\n{\"server\":\"tubekit\"}";
        m_http_request->m_buffer.write(response, strlen(response));
        m_http_request->set_processed(true);
    }

    // write
    bool disconnect = false;
    if (m_http_request->get_recv_over() && false == m_http_request->get_send_over() && m_http_request->get_processed())
    {
        if (false == m_http_request->get_write_eagain())
        {
            m_http_request->buffer_used_len = m_http_request->m_buffer.read(m_http_request->buffer, m_http_request->buffer_size - 1); // read data from write buffer
            m_http_request->buffer[m_http_request->buffer_used_len] = 0;
        }

        bool this_loop_eagain = false;
        while (m_http_request->buffer_used_len > 0 && !disconnect && !this_loop_eagain)
        {
            int ready = 0;
            int last = m_http_request->buffer_used_len;
            while (last > 0)
            {
                int sended = socketfd->send(m_http_request->buffer + ready, last);
                if (sended == -1)
                {
                    if (errno == EINTR)
                    {
                        continue;
                    }
                    else if (errno == EAGAIN)
                    {
                        m_http_request->set_write_eagain(true);
                        this_loop_eagain = true;
                        continue;
                    }
                    disconnect = true; // error
                    break;
                }
                else if (sended == 0) // disconnect or nothing to send
                {
                    disconnect = true;
                    m_http_request->set_send_over(true);
                }
                else
                {
                    last -= sended;
                    ready += sended;
                }
            }
            if (!this_loop_eagain)
            {
                m_http_request->buffer_used_len = m_http_request->m_buffer.read(m_http_request->buffer, m_http_request->buffer_size - 1);
            }
            else
            {
                break;
            }
        }
        if (disconnect || (0 == m_http_request->m_buffer.can_readable_size() && !m_http_request->get_write_eagain()))
        {
            m_http_request->set_send_over(true); // write over
        }
    }

    // continue to epoll_wait
    if (false == m_http_request->get_recv_over())
    {
        handler->attach(socketfd); // continue registe epoll wait write
    }
    else if (false == m_http_request->get_send_over() && m_http_request->get_processed())
    {
        handler->attach(socketfd, true); // wait read
    }
    else
    {
        handler->remove(socketfd); // remove
    }
}

void http_task::test(const request::http_request *m_http_request)
{
    cout << "http_task::test start" << endl;
    cout << m_http_request->method << " " << m_http_request->url << endl;
    // headers
    for_each(m_http_request->headers.begin(), m_http_request->headers.end(),
             [](const pair<string, vector<string>> &m_pair) -> void
             {
                 cout << m_pair.first << ":";
                 for (const string &value : m_pair.second)
                 {
                     cout << value;
                 }
                 cout << endl;
             });
    // body
    cout << "body size : " << m_http_request->body.size() << endl;
    cout << "http_task::test end" << endl;
}