#include <tubekit-log/logger.h>
#include "connection/stream_connection.h"
#include "utility/singleton.h"
#include "socket/socket_handler.h"

using tubekit::connection::stream_connection;
using tubekit::socket::socket_handler;
using tubekit::utility::singleton;

stream_connection::stream_connection(tubekit::socket::socket *socket_ptr) : connection(socket_ptr),
                                                                            should_send_idx(-1),
                                                                            should_send_size(0)
{
}

stream_connection::~stream_connection()
{
}

bool stream_connection::sock2buf(bool &need_task)
{
    need_task = false;
    if (sock2buf_data_len > 0)
    {
        try
        {
            m_recv_buffer.write(sock2buf_inner_buffer, sock2buf_data_len);
        }
        catch (const std::runtime_error &e)
        {
            LOG_ERROR("recv_buffer overflow %s", e.what());
            return false;
        }
        sock2buf_data_len = 0;
    }

    // tag1
    while (true)
    {
        int oper_errno = 0;
        sock2buf_data_len = socket_ptr->recv(sock2buf_inner_buffer, inner_buffer_size, oper_errno);
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
                need_task = true;
                return false;
            }
            sock2buf_data_len = 0;
            continue; // to tag1
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool stream_connection::buf2sock(bool &closed)
{
    closed = false;
    // tag1
    while (true)
    {
        // tag2 insure buf2sock_inner_buffer exist data
        while (should_send_idx < 0)
        {
            int len = -1;
            try
            {
                len = m_send_buffer.read(buf2sock_inner_buffer, inner_buffer_size);
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR(e.what());
                closed = true;
                return false;
            }

            if (len > 0)
            {
                should_send_idx = 0;
                should_send_size = len;
                break; // to tag4
            }

            // tag3
            // no data send in m_send_buffer
            // read package from m_wating_send_pack to m_send_buffer，if data exsit,return true
            while (true)
            {
                uint64_t send_buffer_blank_space = m_send_buffer.blank_space();
                if (send_buffer_blank_space > 0)
                {
                    char temp_buffer[inner_buffer_size];
                    uint64_t reserve_size = send_buffer_blank_space >= inner_buffer_size ? inner_buffer_size : send_buffer_blank_space;

                    int temp_buffer_len = 0;
                    try
                    {
                        temp_buffer_len = m_wating_send_pack.read(temp_buffer, reserve_size);
                    }
                    catch (const std::runtime_error &e)
                    {
                        LOG_ERROR(e.what());
                        closed = true;
                        return false;
                    }

                    if (temp_buffer_len > 0)
                    {
                        try
                        {
                            int writed_len = m_send_buffer.write(temp_buffer, temp_buffer_len);
                            if (writed_len != temp_buffer_len)
                            {
                                LOG_ERROR("write_len[%d] != temp_buffer_len[%d]", writed_len, temp_buffer_len);
                                closed = true;
                                return false;
                            }
                            break; // to tag2
                        }
                        catch (const std::runtime_error &e)
                        {
                            LOG_ERROR(e.what());
                            closed = true;
                            return false;
                        }
                    }
                    else
                    {
                        if (this->write_end_callback)
                        {
                            return this->write_end_callback(*this);
                        }
                        return false; // nothing m_waiting_send_pack to m_send_buffer
                    }
                }
                else
                {
                    LOG_ERROR("send_buffer_blank_space <= 0");
                    closed = true;
                    return false;
                }
            }
        }

        // tag4
        // here buf2sock_inner_buffer exist data
        int oper_errno = 0;
        int len = socket_ptr->send(&buf2sock_inner_buffer[should_send_idx], should_send_size, oper_errno);
        if (0 > len)
        {
            if (oper_errno == EINTR)
            {
                continue; // to tag1
            }
            else if (oper_errno == EAGAIN)
            {
                return true;
            }
            else
            {
                closed = true;
                return false;
            }
        }
        else if (0 == len)
        {
            closed = true;
            return true;
        }
        else
        {
            should_send_size -= len;
            if (should_send_size <= 0)
            {
                should_send_size = 0;
                should_send_idx = -1;
            }
            // to tag1
        }
    }
    closed = true;
    return false;
}

bool stream_connection::send(const char *buffer, size_t buffer_size)
{
    if (buffer == nullptr)
    {
        return false;
    }

    uint64_t len = 0;

    try
    {
        len = m_wating_send_pack.write(buffer, buffer_size);
    }
    catch (const std::runtime_error &e)
    {
        LOG_ERROR("m_wating_send_pack.write return %lu %lu %s", buffer_size, len, e.what());
        return false;
    }

    if (get_gid() > 0)
    {
        singleton<socket_handler>::instance()->do_task(get_gid(), false, true);
    }
    else
    {
        LOG_ERROR("get_gid()<=0");
        return false;
    }

    return true;
}

void stream_connection::on_mark_close()
{
    m_send_buffer.clear();      // GC
    m_recv_buffer.clear();      // GC
    m_wating_send_pack.clear(); // GC
    singleton<socket_handler>::instance()->do_task(get_gid(), false, true);
}

void stream_connection::reuse()
{
    connection::reuse();

    constexpr uint64_t mem_buffer_size_max = inner_buffer_size + 2; // 2MB
    m_send_buffer.clear();                                          // GC
    m_send_buffer.set_limit_max(mem_buffer_size_max);
    m_recv_buffer.clear(); // GC
    m_recv_buffer.set_limit_max(mem_buffer_size_max);
    m_wating_send_pack.clear(); // GC
    m_wating_send_pack.set_limit_max(mem_buffer_size_max);

    this->should_send_idx = -1;
    this->should_send_size = 0;
    this->sock2buf_data_len = 0;

    write_end_callback = nullptr;
}