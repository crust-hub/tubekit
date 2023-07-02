#include "connection/http_connection.h"

using namespace std;
using namespace tubekit::connection;

http_connection::http_connection(tubekit::socket::socket *socket_ptr) : m_buffer(10240),
                                                                        socket_ptr(socket_ptr),
                                                                        recv_end(false),
                                                                        process_end(false),
                                                                        buffer_used_len(0),
                                                                        buffer_start_use(0),
                                                                        response_end(false),
                                                                        everything_end(false)
{
    http_parser_init(&m_http_parser, HTTP_REQUEST);
    m_http_parser.data = this;
}

http_connection::~http_connection()
{
    if (destory_callback)
    {
        destory_callback(*this);
    }
}

http_parser *http_connection::get_parser()
{
    return &m_http_parser;
}

void http_connection::add_header(const std::string &key, const std::string &value)
{
    auto res = headers.find(key);
    if (res == headers.end())
    {
        vector<string> m_vec;
        headers[key] = m_vec;
    }
    headers[key].push_back(value);
}

void http_connection::add_to_body(const char *data, const size_t len)
{
    if (len == 0 || data == nullptr)
    {
        return;
    }
    body.insert(body.end(), data, data + len);
}

void http_connection::add_chunk(const std::vector<char> &chunk)
{
    chunks.push_back(chunk);
}

void http_connection::set_url(const char *url, size_t url_len)
{
    this->url = string(url, url_len);
}

void http_connection::set_recv_end(bool recv_end)
{
    this->recv_end = recv_end;
}

bool http_connection::get_recv_end()
{
    return recv_end;
}

void http_connection::set_process_end(bool process_end)
{
    this->process_end = process_end;
}

bool http_connection::get_process_end()
{
    return process_end;
}

void http_connection::set_response_end(bool response_end)
{
    this->response_end = response_end;
}

bool http_connection::get_response_end()
{
    return response_end;
}

bool http_connection::set_everything_end(bool everything_end)
{
    this->everything_end = everything_end;
}

bool http_connection::get_everything_end()
{
    return everything_end;
}

ostream &operator<<(ostream &os, const http_connection &m_http_connection)
{
    return os;
}