#include "connection/stream_connection.h"

using tubekit::connection::stream_connection;

stream_connection::stream_connection(tubekit::socket::socket *socket_ptr) : socket_ptr(socket_ptr),
                                                                            m_send_buffer(2048000),
                                                                            m_recv_buffer(2048000),
                                                                            connection_state(stream_connection::state::RECV)
{
}

stream_connection::~stream_connection()
{
}