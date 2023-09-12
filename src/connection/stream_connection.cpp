#include <tubekit-log/logger.h>
#include "connection/stream_connection.h"
#include "utility/singleton.h"
#include "socket/socket_handler.h"

using tubekit::connection::stream_connection;
using tubekit::socket::socket_handler;
using tubekit::utility::singleton;

stream_connection::stream_connection(tubekit::socket::socket *socket_ptr) : socket_ptr(socket_ptr),
                                                                            m_send_buffer(2048),
                                                                            m_recv_buffer(2048),
                                                                            m_wating_send_pack(2048)
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
        // static char buffer no data
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
                continue;
            }

            // no data send in m_send_buffer
            // read package from m_wating_send_pack to m_send_buffer，if data exsit,return true
            {
                bool have_data = false;
                while (true)
                {
                    char temp_buffer[1024];
                    int temp_buffer_len = 0;
                    try
                    {
                        temp_buffer_len = m_wating_send_pack.read(temp_buffer, 1024);
                    }
                    catch (const std::runtime_error &e)
                    {
                        LOG_ERROR(e.what());
                    }
                    if (temp_buffer_len > 0)
                    {
                        have_data = true;
                        try
                        {
                            int writed_len = m_send_buffer.write(temp_buffer, temp_buffer_len);
                            if (writed_len != temp_buffer_len)
                            {
                                LOG_ERROR("write_len[%d] != temp_buffer_len[%d]", writed_len, temp_buffer_len);
                            }
                        }
                        catch (const std::runtime_error &e)
                        {
                            LOG_ERROR(e.what());
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                return have_data;
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

// bool stream_connection::send(char *buffer, size_t buffer_size)
// {
//     if (buffer == nullptr)
//     {
//         return false;
//     }
//     try
//     {
//         u_int64_t len = m_wating_send_pack.write(buffer, buffer_size);
//     }
//     catch (...)
//     {
//         return false;
//     }
//     if (socket_ptr)
//     {
//         singleton<socket_handler>::instance()->attach(socket_ptr, true);
//     }
//     return true;
// }
