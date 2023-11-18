#include "connection/websocket_connection.h"

using namespace std;
using namespace tubekit::connection;

websocket_connection::websocket_connection(tubekit::socket::socket *socket_ptr) : connection(socket_ptr),
                                                                                  connected(false),
                                                                                  http_processed(false),
                                                                                  buffer_used_len(0),
                                                                                  buffer_start_use(0),
                                                                                  m_recv_buffer(20480)
{
    http_parser_init(&m_http_parser, HTTP_REQUEST);
    m_http_parser.data = this;
}

websocket_connection::~websocket_connection()
{
    if (destory_callback)
    {
        destory_callback(*this);
    }
}

void websocket_connection::on_mark_close()
{
}

bool websocket_connection::get_connected()
{
    return this->connected;
}

void websocket_connection::set_connected(bool connected)
{
    this->connected = connected;
}

http_parser *websocket_connection::get_parser()
{
    return &this->m_http_parser;
}

void websocket_connection::add_header(const std::string &key, const std::string &value)
{
    auto res = headers.find(key);
    if (res == headers.end())
    {
        vector<string> m_vec;
        headers[key] = m_vec;
    }
    headers[key].push_back(value);
}