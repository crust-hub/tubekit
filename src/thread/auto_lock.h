#pragma once
#include "thread/mutex.h"

namespace tubekit
{
    namespace thread
    {
        /**
         * @brief lock mutex in constructor,unlock in destructor
         *
         */
        class auto_lock
        {
        public:
            auto_lock(mutex &mutex);
            ~auto_lock();

        private:
            mutex &m_mutex;
        };
    }
}