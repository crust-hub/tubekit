#include <tubekit-log/logger.h>
#include "connection/stream_connection.h"

using tubekit::connection::stream_connection;

stream_connection::stream_connection(tubekit::socket::socket *socket_ptr) : socket_ptr(socket_ptr),
                                                                            m_send_buffer(204800),
                                                                            m_recv_buffer(204800),
                                                                            connection_state(stream_connection::state::WAIT_RECV)
{
}

stream_connection::~stream_connection()
{
}

bool stream_connection::sock2buf()
{
    static char buffer[1024];
    while (true)
    {
        int len = socket_ptr->recv(buffer, 1024);
        if (len == -1 && errno == EAGAIN)
        {
            return true;
        }
        else if (len == -1 && errno == EINTR)
        {
            continue;
        }
        else if (len > 0)
        {
            try
            {
                m_recv_buffer.write(buffer, len);
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR(e.what());
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool stream_connection::buf2sock()
{
    static char buffer[1024];
    static int shouldSendIndex = -1;
    static int shouldSendSize = 0;
    while (true)
    {
        if (shouldSendIndex < 0)
        {
            int len = -1;
            try
            {
                len = m_send_buffer.read(buffer, 1024);
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR(e.what());
            }
            if (len > 0)
            {
                shouldSendIndex = 0;
                shouldSendSize = len;
            }
            else
            {
                // no data send
                return false;
            }
        }
        int len = socket_ptr->send(&buffer[shouldSendIndex], shouldSendSize);
        if (0 > len)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else if (errno == EAGAIN)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (0 == len)
        {
            return false;
        }
        else
        {
            shouldSendSize -= len;
            if (shouldSendSize <= 0)
            {
                shouldSendSize = 0;
                shouldSendIndex = -1;
            }
        }
    }
    return false;
}