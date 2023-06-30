#include "request/stream_request.h"

using tubekit::request::stream_request;

stream_request::stream_request(int socket_fd) : socket_fd(socket_fd),
                                                m_send_buffer(1024),
                                                m_recv_buffer(1024),
                                                request_state(stream_request::state::RECV)
{
}

stream_request::~stream_request()
{
}