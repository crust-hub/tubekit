#pragma once
#include "thread/mutex.h"
#include <cstdint>

namespace tubekit
{
    namespace thread
    {
        class task
        {
        public:
            task(uint64_t gid);
            virtual ~task();

            uint64_t get_gid();
            void set_gid(uint64_t gid);

            virtual void run() = 0;     // pure virtual function
            virtual void destroy() = 0; // interface
            virtual bool compare(task *other);

        protected:
            uint64_t m_gid{0};
        };
    }
}