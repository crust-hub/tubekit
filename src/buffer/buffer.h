#pragma once
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <mutex>
#include <stdexcept>

namespace tubekit
{
    namespace buffer
    {
        class buffer
        {
        public:
            buffer(u_int64_t limit_max);
            ~buffer();
            time_t get_last_read_gap();
            time_t get_last_write_gap();
            u_int64_t read(char *dest, u_int64_t size) noexcept(false);
            u_int64_t write(const char *source, u_int64_t size) noexcept(false);

        private:
            u_int64_t m_limit_max;
            time_t m_last_read;
            time_t m_last_write;
            u_int64_t m_size;
            char *m_read_ptr;
            char *m_write_ptr;
            char *m_buffer;
            std::mutex m_mutex;

        private:
            bool check_and_write(const char *source, u_int64_t size);
            void move_to_before();
            u_int64_t after_size();
        };
    }
}
