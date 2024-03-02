#include "buffer.h"
#include <iostream>

using tubekit::buffer::buffer;

buffer::buffer(u_int64_t limit_max)
{
    m_size = limit_max < 1024 ? limit_max : 1024;
    m_limit_max = limit_max;
    m_buffer = nullptr;
    m_buffer = (char *)malloc(m_size);
    m_read_ptr = m_buffer;
    m_write_ptr = m_buffer;
    m_last_read = time(nullptr);
    m_last_write = time(nullptr);
}

buffer::~buffer()
{
    if (m_buffer)
    {
        free(m_buffer);
    }
}

time_t buffer::get_last_read_gap()
{
    time_t now = time(nullptr);
    return now - m_last_read;
}

time_t buffer::get_last_write_gap()
{
    time_t now = time(nullptr);
    return now - m_last_write;
}

u_int64_t buffer::read(char *dest, u_int64_t size)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (dest == nullptr || size == 0)
    {
        throw std::runtime_error("dest == nullptr || size == 0");
    }
    // read size of bytes from m_buffer
    u_int64_t can_readable = 0;
    get_readable_size(can_readable);
    if (size <= can_readable)
    {
        memcpy(dest, m_read_ptr, size);
        m_read_ptr += size;
        // update m_last_read
        m_last_read = time(nullptr);
        return size;
    }
    else
    {
        memcpy(dest, m_read_ptr, can_readable);
        m_read_ptr += can_readable;
        // update m_last_read
        m_last_read = time(nullptr);
        return can_readable;
    }
}

u_int64_t buffer::write(const char *source, u_int64_t size)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (source == nullptr || size == 0)
    {
        throw std::runtime_error("dest == nullptr || size == 0");
    }
    // check buffer size
    if (!check_and_write(source, size))
    {
        // move to before
        move_to_before();
        if (check_and_write(source, size))
        {
            m_last_write = time(nullptr);
        }
        else
        {
            // realloc
            u_int64_t should_add_size = size - after_size();
            if (should_add_size + m_size > m_limit_max)
            {
                throw std::runtime_error("should_add_size + m_size > m_limit_max");
            }
            char *new_buffer = (char *)realloc(m_buffer, m_size + should_add_size);
            if (new_buffer)
            {
                // Adjust read and write pointers
                size_t read_ptr_gap = m_read_ptr - m_buffer;
                size_t write_ptr_gap = m_write_ptr - m_buffer;
                m_buffer = new_buffer;
                m_read_ptr = m_buffer + read_ptr_gap;
                m_write_ptr = m_buffer + write_ptr_gap;
                m_size = m_size + should_add_size;
            }
            else
            {
                throw std::runtime_error("realloc error");
            }
            if (check_and_write(source, size))
            {
                m_last_write = time(nullptr);
            }
        }
    }
    return size;
}

bool buffer::check_and_write(const char *source, u_int64_t size)
{
    u_int64_t m_after_size = after_size();
    if (size <= m_after_size)
    {
        memcpy(m_write_ptr, source, size);
        m_write_ptr = m_write_ptr + size;
        m_last_write = time(nullptr);
        return true;
    }
    return false;
}

void buffer::move_to_before()
{
    u_int64_t readable = 0;
    get_readable_size(readable);
    memmove(m_buffer, m_read_ptr, readable);
    m_write_ptr = m_buffer + readable;
    m_read_ptr = m_buffer;
}

u_int64_t buffer::after_size()
{
    char *last_buffer_ptr = m_buffer + m_size;
    u_int64_t after_size = last_buffer_ptr - m_write_ptr;
    return after_size;
}

u_int64_t buffer::can_readable_size()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    u_int64_t size;
    get_readable_size(size);
    return size;
}

void buffer::get_readable_size(u_int64_t &out)
{
    out = m_write_ptr - m_read_ptr;
}

void buffer::set_limit_max(u_int64_t limit_max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_limit_max = limit_max;
}

u_int64_t buffer::get_limit_max()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_limit_max;
}

void buffer::clear()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_read_ptr = m_buffer;
    m_write_ptr = m_buffer;
}

char *buffer::get_read_ptr()
{
    return m_read_ptr;
}

u_int64_t buffer::copy_all(char *out, u_int64_t out_len)
{
    if (!out)
    {
        return 0;
    }
    std::lock_guard<std::mutex> guard(m_mutex);
    uint64_t all_bytes = 0;
    get_readable_size(all_bytes);
    if (out_len < all_bytes)
    {
        return 0;
    }
    memcpy(out, get_read_ptr(), all_bytes);
    return all_bytes;
}

bool buffer::read_ptr_move_n(u_int64_t n)
{
    if (n == 0)
    {
        return true;
    }
    std::lock_guard<std::mutex> guard(m_mutex);
    uint64_t all_bytes = 0;
    get_readable_size(all_bytes);
    if (n > all_bytes)
    {
        return false;
    }
    m_read_ptr = m_read_ptr + n;
    return true;
}