#include "connection/websocket_connection.h"

using namespace std;
using namespace tubekit::connection;

websocket_connection::websocket_connection(tubekit::socket::socket *socket_ptr) : connection(socket_ptr),
                                                                                  connected(false)
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