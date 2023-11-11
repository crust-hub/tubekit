#include "connection/websocket_connection.h"

using namespace std;
using namespace tubekit::connection;

websocket_connection::websocket_connection(tubekit::socket::socket *socket_ptr) : connection(socket_ptr)
{
}

websocket_connection::~websocket_connection()
{
}

void websocket_connection::on_mark_close()
{
}