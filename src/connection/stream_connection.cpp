#include "connection/stream_connection.h"

using tubekit::connection::stream_connection;

stream_connection::stream_connection(int socket_fd) : socket_fd(socket_fd),
                                                m_send_buffer(1024),
                                                m_recv_buffer(1024),
                                                connection_state(stream_connection::state::RECV)
{
}

stream_connection::~stream_connection()
{
}