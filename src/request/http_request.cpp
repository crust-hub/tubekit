#include "request/http_request.h"

using namespace std;
using namespace tubekit::request;

http_request::http_request(int socket_fd) : m_buffer(1024),
                                            socket_fd(socket_fd),
                                            recv_over(false),
                                            send_over(false),
                                            processed(false),
                                            write_eagain(false),
                                            buffer_used_len(0)
{
    http_parser_init(&m_http_parser, HTTP_REQUEST);
    m_http_parser.data = this;
}

http_request::~http_request()
{
}

http_parser *http_request::get_parser()
{
    return &m_http_parser;
}

void http_request::add_header(const std::string &key, const std::string &value)
{
    auto res = headers.find(key);
    if (res == headers.end())
    {
        vector<string> m_vec;
        headers[key] = m_vec;
    }
    headers[key].push_back(value);
}

void http_request::add_to_body(const char *data, const size_t len)
{
    if (len == 0 || data == nullptr)
    {
        return;
    }
    body.insert(body.end(), data, data + len);
}

void http_request::add_chunk(const std::vector<char> &chunk)
{
    chunks.push_back(chunk);
}

void http_request::set_url(const char *url, size_t url_len)
{
    this->url = string(url, url_len);
}

ostream &operator<<(ostream &os, const http_request &m_http_request)
{
    return os;
}

void http_request::set_recv_over(bool recv_over)
{
    this->recv_over = recv_over;
}

bool http_request::get_recv_over()
{
    return recv_over;
}

void http_request::set_send_over(bool send_over)
{
    this->send_over = send_over;
}

bool http_request::get_send_over()
{
    return this->send_over;
}

void http_request::set_processed(bool processed)
{
    this->processed = processed;
}

bool http_request::get_processed()
{
    return this->processed;
}

void http_request::set_write_eagain(bool write_eagain)
{
    this->write_eagain = write_eagain;
}

bool http_request::get_write_eagain()
{
    return write_eagain;
}