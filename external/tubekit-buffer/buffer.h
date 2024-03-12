#pragma once
#include <cstdlib>
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
            buffer(uint64_t limit_max);
            ~buffer();

            uint64_t read(char *dest, uint64_t size) noexcept(false);
            uint64_t write(const char *source, uint64_t size) noexcept(false);
            uint64_t can_readable_size();
            void set_limit_max(uint64_t limit_max);
            uint64_t get_limit_max();
            void clear();
            uint64_t copy_all(char *out, uint64_t out_len);
            bool read_ptr_move_n(uint64_t n);

        private:
            uint64_t m_limit_max;
            uint64_t m_size;
            char *m_read_ptr;
            char *m_write_ptr;
            char *m_buffer;
            std::mutex m_mutex;

        private:
            bool check_and_write(const char *source, uint64_t size);
            void get_readable_size(uint64_t &out);
            void move_to_before();
            uint64_t after_size();
            char *get_read_ptr();
        };
    }
}
