#include "buffer.h"
#include <iostream>

using tubekit::buffer::buffer;

buffer::buffer(uint64_t limit_max)
{
    m_size = limit_max < 1024 ? limit_max : 1024;
    m_limit_max = limit_max;
    m_buffer = nullptr;
    m_buffer = (char *)malloc(m_size);
    if(nullptr == m_buffer)
    {
        throw std::runtime_error("buffer::buffer malloc failed");
    }
    m_read_ptr = m_buffer;
    m_write_ptr = m_buffer;
}

buffer::~buffer()
{
    if (m_buffer)
    {
        free(m_buffer);
    }
}

uint64_t buffer::read(char *dest, uint64_t size)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (dest == nullptr || size == 0)
    {
        throw std::runtime_error("dest == nullptr || size == 0");
    }
    // read size of bytes from m_buffer
    uint64_t can_readable = 0;
    get_readable_size(can_readable);
    if (size <= can_readable)
    {
        memcpy(dest, m_read_ptr, size);
        m_read_ptr += size;
        return size;
    }
    else
    {
        memcpy(dest, m_read_ptr, can_readable);
        m_read_ptr += can_readable;
        return can_readable;
    }
}

uint64_t buffer::write(const char *source, uint64_t size)
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
        }
        else
        {
            // realloc
            uint64_t should_add_size = size - after_size();
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
            }
        }
    }
    return size;
}

bool buffer::check_and_write(const char *source, uint64_t size)
{
    uint64_t m_after_size = after_size();
    if (size <= m_after_size)
    {
        memcpy(m_write_ptr, source, size);
        m_write_ptr = m_write_ptr + size;
        return true;
    }
    return false;
}

void buffer::move_to_before()
{
    uint64_t readable = 0;
    get_readable_size(readable);
    memmove(m_buffer, m_read_ptr, readable);
    m_write_ptr = m_buffer + readable;
    m_read_ptr = m_buffer;
}

uint64_t buffer::after_size()
{
    char *last_buffer_ptr = m_buffer + m_size;
    uint64_t after_size = last_buffer_ptr - m_write_ptr;
    return after_size;
}

uint64_t buffer::can_readable_size()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    uint64_t size;
    get_readable_size(size);
    return size;
}

void buffer::get_readable_size(uint64_t &out)
{
    out = m_write_ptr - m_read_ptr;
}

void buffer::set_limit_max(uint64_t limit_max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_limit_max = limit_max;
}

uint64_t buffer::get_limit_max()
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

uint64_t buffer::copy_all(char *out, uint64_t out_len)
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

bool buffer::read_ptr_move_n(uint64_t n)
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

char *buffer::force_get_read_ptr()
{
    return get_read_ptr();
}

uint64_t buffer::blank_space()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    uint64_t u_after_size = after_size();
    uint64_t can_promote_bytes = m_limit_max - m_size;
    return u_after_size + can_promote_bytes;
}