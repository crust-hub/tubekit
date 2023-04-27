#include <string>
#include <sstream>
#include <http_parser.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "http_task.h"
#include "socket/socket_handler.h"
#include "utility/singleton_template.h"
#include "http/http_session.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::http;

http_task::http_task(tubekit::socket::socket *m_socket) : task(m_socket)
{
}

http_task::~http_task()
{
}

void http_task::destroy()
{
    delete this;
}

void http_task::run()
{
    socket_handler *handler = singleton_template<socket_handler>::instance();
    socket::socket *socketfd = static_cast<socket::socket *>(m_data);
    socketfd->delete_ptr_hook = [](void *ptr) -> void
    {
        if (ptr != nullptr)
        {
            http::session *http_session = (http::session *)ptr;
            delete http_session;
        }
    };
    if (socketfd->ptr == nullptr)
    {
        http::session *http_session = new http::session(socketfd->get_fd());
        socketfd->ptr = (void *)http_session;
    }
    http::session *http_session = (http::session *)socketfd->ptr;
    http_parser_settings settings;
    settings.on_message_begin = [](http_parser *parser) -> int
    {
        http::session *m_session = static_cast<http::session *>(parser->data);
        http_method method = (http_method)parser->method;
        m_session->method = http_method_str(method);
        return 0;
    };

    settings.on_url = [](http_parser *parser, const char *at, size_t length) -> int
    {
        http::session *m_session = static_cast<http::session *>(parser->data);
        m_session->url = std::string(at, length);
        return 0; // allowed
        // return -1;// reject this connection
    };

    settings.on_status = [](http_parser *parser, const char *at, size_t length) -> int
    {
        // parser reponses
        return 0;
    };

    settings.on_header_field = [](http_parser *parser, const char *at, size_t length) -> int
    {
        http::session *m_session = static_cast<http::session *>(parser->data);
        m_session->head_field_tmp = std::string(at, length);
        return 0;
    };

    settings.on_header_value = [](http_parser *parser, const char *at, size_t length) -> int
    {
        http::session *m_session = static_cast<http::session *>(parser->data);
        std::string value(at, length);
        m_session->add_header(m_session->head_field_tmp, value);
        return 0;
    };

    settings.on_headers_complete = [](http_parser *parser) -> int
    {
        // http::session *m_session = static_cast<http::session *>(parser->data);
        // std::cout << "on_headers_complete" << std::endl;
        return 0;
    };

    settings.on_body = [](http_parser *parser, const char *at, size_t length) -> int
    {
        http::session *http_session = static_cast<http::session *>(parser->data);
        http_session->add_to_body(at, length);
        return 0;
    };

    settings.on_message_complete = [](http_parser *parser) -> int
    {
        http::session *m_session = static_cast<http::session *>(parser->data);
        m_session->set_over(true);
        //  std::cout << "on_message_complete" << std::endl;
        return 0;
    };

    settings.on_chunk_header = [](http_parser *parser) -> int
    {
        // http::session *m_session = static_cast<http::session *>(parser->data);
        std::cout << "on_chunk_header" << std::endl;
        return 0;
    };

    settings.on_chunk_complete = [](http_parser *parser) -> int
    {
        // http::session *m_session = static_cast<http::session *>(parser->data);
        std::cout << "on_chunk_complete" << std::endl;
        return 0;
    };

    // read
    int len = 0;
    while (1)
    {
        len = socketfd->recv(http_session->buffer, http_session->buffer_size);
        if (len == -1 && errno == EAGAIN)
        {
            // std::cout << "EAGAIN :" << strerror(errno) << std::endl;
            break;
        }
        else if (len == -1 && errno == EWOULDBLOCK)
        {
            // std::cout << "EWOULDBLOCK :" << strerror(errno) << std::endl;
            break;
        }
        else if (len == -1 && errno == EINTR) // error interupt
        {
            continue;
        }
        else if (len == 0) // noting recv later
        {
            // std::cout << "len==0" << std::endl;
            http_session->set_over(true);
            break;
        }
        else if (len > 0)
        {
            int nparsed = http_parser_execute(http_session->get_parser(), &settings, http_session->buffer, len);
            if (http_session->get_parser()->upgrade)
            {
            }
            else if (nparsed != len) // error
            {
                break;
            }
        }
    } // while(1)
    if (http_session->get_over())
    {
        test(http_session);
        const char *response = "HTTP/1.1 200 OK\r\nServer: tubekit\r\nContent-Type: text/json;\r\n\r\n";
        // http response....
        int last = strlen(response);
        int ready = 0;
        while (last > 0)
        {
            int len = socketfd->send(response + ready, last);
            if (len == -1)
            {
            }
            else if (len == 0)
            {
            }
            else
            {
                last -= len;
                ready += len;
            }
        }
        handler->remove(socketfd); // detach and back to object poll
    }
    else
    {
        handler->attach(socketfd); // continue registe epoll
    }
}

void http_task::test(const http::session *m_session)
{
    cout << "http_task::test start" << endl;
    cout << m_session->method << m_session->url << endl;
    // headers
    for_each(m_session->headers.begin(), m_session->headers.end(), [](const pair<string, vector<string>> &m_pair) -> void
             {
                cout << m_pair.first << ":"; 
                for(const string&value:m_pair.second){
                    cout << value;
                }
                cout<<endl; });
    // body
    cout << "body size : " << m_session->body.size() << endl;
    cout << "http_task::test end" << endl;
}