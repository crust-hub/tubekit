#include <string>
#include <sstream>
#include <http_parser.h>
#include <memory.h>
#include <vector>
#include <iostream>

#include "http_task.h"
#include "socket/socket_handler.h"
#include "utility/singleton_template.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;

typedef struct
{
    int sock;
    char *buffer;
    size_t buf_len;
    shared_ptr<vector<char>> buffer_all;
} custom_data_t;

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
    //===> http_parser
    int nparsed = 0;
    custom_data_t *my_data = new custom_data_t;
    my_data->buffer_all = make_shared<vector<char>>();
    my_data->buf_len = 1024;
    my_data->buffer = new char[my_data->buf_len];
    my_data->sock = socketfd->get_fd();
    http_parser *parser = new http_parser;
    http_parser_init(parser, HTTP_REQUEST);
    parser->data = my_data;
    http_parser_settings settings;

    settings.on_message_begin = [](http_parser *parser) -> int
    {
        std::cout << "on_message_begin" << std::endl;
        return 0;
    };

    settings.on_url = [](http_parser *parser, const char *at, size_t length) -> int
    {
        std::cout << "on_url ";
        char str[1024];
        bzero(str, sizeof(str));
        memcpy(str, at, length);
        str[length] = 0;
        std::cout << str << std::endl;
        return 0; // allowed
        // return -1;// reject this connection
    };

    settings.on_status = [](http_parser *parser, const char *at, size_t length) -> int
    {
        std::cout << "on_status ";
        char str[1024];
        bzero(str, sizeof(str));
        memcpy(str, at, length);
        str[length] = 0;
        std::cout << str << std::endl;
        return 0;
    };

    settings.on_header_field = [](http_parser *parser, const char *at, size_t length) -> int
    {
        std::cout << "on_header_field ";
        char str[1024];
        bzero(str, sizeof(str));
        memcpy(str, at, length);
        str[length] = 0;
        std::cout << str << std::endl;
        return 0;
    };

    settings.on_header_value = [](http_parser *parser, const char *at, size_t length) -> int
    {
        std::cout << "on_header_value ";
        char str[1024];
        bzero(str, sizeof(str));
        memcpy(str, at, length);
        str[length] = 0;
        std::cout << str << std::endl;
        return 0;
    };

    settings.on_headers_complete = [](http_parser *parser) -> int
    {
        std::cout << "on_headers_complete" << std::endl;
        return 0;
    };

    settings.on_body = [](http_parser *parser, const char *at, size_t length) -> int
    {
        std::cout << "on_body " << length << std::endl;
        char str[1024];
        bzero(str, sizeof(str));
        memcpy(str, at, length);
        str[length] = 0;
        std::cout << str << std::endl;
        return 0;
    };

    settings.on_message_complete = [](http_parser *parser) -> int
    {
        std::cout << ((custom_data_t *)parser->data)->buffer_all->size() << std::endl;
        std::cout << "on_message_complete" << std::endl;
        return 0;
    };

    settings.on_chunk_header = [](http_parser *parser) -> int
    {
        std::cout << "on_chunk_header" << std::endl;
        return 0;
    };

    settings.on_chunk_complete = [](http_parser *parser) -> int
    {
        std::cout << "on_chunk_complete" << std::endl;
        return 0;
    };

    // read
    int len = 0;
    while ((len = socketfd->recv(my_data->buffer, my_data->buf_len)) > 0)
    {
        for (int i = 0; i < len; i++)
        {
            my_data->buffer_all->push_back(my_data->buffer[i]);
        }
        nparsed = http_parser_execute(parser, &settings, my_data->buffer, len);
        if (parser->upgrade)
        {
        }
        else if (nparsed != len)
        {
            handler->attach(socketfd);
            break;
        }
    }
    delete parser;
    delete[] my_data->buffer;
    delete my_data;
    handler->remove(socketfd);
}
