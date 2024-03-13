#include <tubekit-log/logger.h>
#include "connection/stream_connection.h"
#include "utility/singleton.h"
#include "socket/socket_handler.h"

using tubekit::connection::stream_connection;
using tubekit::socket::socket_handler;
using tubekit::utility::singleton;

stream_connection::stream_connection(tubekit::socket::socket *socket_ptr) : connection(socket_ptr),
                                                                            m_send_buffer(20480),
                                                                            m_recv_buffer(20480),
                                                                            m_wating_send_pack(20480),
                                                                            should_send_idx(-1),
                                                                            should_send_size(0)
{
}

stream_connection::~stream_connection()
{
}

bool stream_connection::sock2buf()
{
    if (sock2buf_data_len > 0)
    {
        try
        {
            m_recv_buffer.write(sock2buf_inner_buffer, sock2buf_data_len);
        }
        catch (const std::runtime_error &e)
        {
            LOG_ERROR("%s", e.what());
            return false;
        }
        sock2buf_data_len = 0;
    }

    while (true)
    {
        int oper_errno = 0;
        sock2buf_data_len = socket_ptr->recv(sock2buf_inner_buffer, 1024, oper_errno);
        if (sock2buf_data_len == -1 && oper_errno == EAGAIN)
        {
            return true;
        }
        else if (sock2buf_data_len == -1 && oper_errno == EINTR)
        {
            continue;
        }
        else if (sock2buf_data_len > 0)
        {
            try
            {
                m_recv_buffer.write(sock2buf_inner_buffer, sock2buf_data_len);
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR("%s", e.what());
                return false;
            }
            sock2buf_data_len = 0;
            return true;
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
    while (true)
    {
        if (should_send_idx < 0)
        {
            int len = -1;
            try
            {
                len = m_send_buffer.read(buf2sock_inner_buffer, 1024);
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR(e.what());
            }

            if (len > 0)
            {
                should_send_idx = 0;
                should_send_size = len;
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
        int oper_errno = 0;
        int len = socket_ptr->send(&buf2sock_inner_buffer[should_send_idx], should_send_size, oper_errno);
        if (0 > len)
        {
            if (oper_errno == EINTR)
            {
                continue;
            }
            else if (oper_errno == EAGAIN)
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
            should_send_size -= len;
            if (should_send_size <= 0)
            {
                should_send_size = 0;
                should_send_idx = -1;
            }
        }
    }
    return false;
}

bool stream_connection::send(const char *buffer, size_t buffer_size)
{
    if (buffer == nullptr)
    {
        return false;
    }

    // uint64_t len = 0;

    try
    {
        // len =
        m_wating_send_pack.write(buffer, buffer_size);
    }
    catch (const std::runtime_error &e)
    {
        // LOG_ERROR("m_wating_send_pack.write(buffer, %d) return %d %s", buffer_size, len, e.what());
        return false;
    }

    if (socket_ptr)
    {
        singleton<socket_handler>::instance()->do_task(socket_ptr, false, true);
    }
    else
    {
        LOG_ERROR("socket_ptr is nullptr");
        return false;
    }

    return true;
}

void stream_connection::on_mark_close()
{
    singleton<socket_handler>::instance()->do_task(socket_ptr, false, true);
}

void stream_connection::reuse()
{
    connection::reuse();
    m_send_buffer.clear();
    m_recv_buffer.clear();
    m_wating_send_pack.clear();
    this->should_send_idx = -1;
    this->should_send_size = 0;
    this->sock2buf_data_len = 0;
}