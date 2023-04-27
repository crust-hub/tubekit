#include "http_session.h"

using namespace std;
using namespace tubekit::http;

session::session(int socket_fd) : buffer_size(1024), socket_fd(socket_fd), over(false)
{
    m_http_parser = new http_parser;
    http_parser_init(m_http_parser, HTTP_REQUEST);
    m_http_parser->data = this;
    buffer = new char[buffer_size];
}

session::~session()
{
    // std::cout << "session free" << std::endl;
    delete[] buffer;
    delete m_http_parser;
}

http_parser *session::get_parser()
{
    return m_http_parser;
}

void session::add_header(const std::string &key, const std::string &value)
{
    auto res = headers.find(key);
    if (res == headers.end())
    {
        vector<string> m_vec;
        headers[key] = m_vec;
    }
    headers[key].push_back(value);
}

void session::add_to_body(const char *data, const size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        body.push_back(data[i]);
    }
}

void session::add_chunk(const std::vector<char> &chunk)
{
    chunks.push_back(chunk);
}

void session::set_url(const char *url, size_t url_len)
{
    this->url = string(url, url_len);
}

ostream &operator<<(ostream &os, const session &m_session)
{
    return os;
}

void session::set_over(bool over)
{
    this->over = over;
}

bool session::get_over()
{
    return over;
}