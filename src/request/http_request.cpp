#include "request/http_request.h"

using namespace std;
using namespace tubekit::request;

http_request::http_request(int socket_fd) : m_buffer(1024), buffer_size(1024), socket_fd(socket_fd), over(false)
{
    m_http_parser = new http_parser;
    http_parser_init(m_http_parser, HTTP_REQUEST);
    m_http_parser->data = this;
    buffer = new char[buffer_size];
}

http_request::~http_request()
{
    // std::cout << "session free" << std::endl;
    delete buffer;
    delete m_http_parser;
}

http_parser *http_request::get_parser()
{
    return m_http_parser;
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

void http_request::set_over(bool over)
{
    this->over = over;
}

bool http_request::get_over()
{
    return over;
}
