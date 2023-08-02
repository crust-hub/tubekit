#include "connection/stream_connection.h"

using tubekit::connection::stream_connection;

stream_connection::stream_connection(tubekit::socket::socket *socket_ptr) : socket_ptr(socket_ptr),
                                                                            m_send_buffer(204800),
                                                                            m_recv_buffer(204800),
                                                                            connection_state(stream_connection::state::RECV)
{
}

stream_connection::~stream_connection()
{
}