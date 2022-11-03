#include "http_session.h"

using namespace std;
using namespace tubekit::http;

session::session()
{
}

session::~session()
{
}

void session::add_header(const char *key, const size_t key_len, const char *value, const size_t value_len)
{
    headers.insert(pair<string, string>(string(key, key_len), string(value, value_len)));
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
    os << m_session.url << endl;
    for (auto &header : m_session.headers)
    {
        os << header.first << " " << header.second << endl;
    }
    os << "\r\n";
    if (!m_session.body.empty())
    {
        for (auto &ch : m_session.body)
        {
            os << ch;
        }
        os << "\r\n";
    }
    if (!m_session.chunks.empty())
    {
        for (auto &chunk : m_session.chunks)
        {
            for (auto &ch : chunk)
            {
                cout << ch;
            }
        }
        os << "\r\n";
    }
    return os;
}

int main(void)
{
    session m_session;
    m_session.set_url("123", 3);
    m_session.add_to_body("123", 3);
    m_session.add_to_body("456", 3);
    m_session.add_header("name", 4, "asda", 4);
    m_session.add_header("fsdf", 4, "asda", 4);
    m_session.add_header("fdbf", 4, "asda", 4);
    m_session.add_chunk({'1', '2', '3', '4'});
    m_session.add_chunk({'5', '6', '7', '8'});
    cout << m_session << endl;
    return 0;
}
