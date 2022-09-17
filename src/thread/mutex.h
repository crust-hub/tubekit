#pragma once
#include <pthread.h>

namespace tubekit
{
    namespace thread
    {
        class mutex
        {
            friend class condition;

        public:
            mutex();
            ~mutex();

            /**
             * @brief blocking lock
             *
             * @return int
             */
            int lock();

            /**
             * @brief non-blocking lock
             *
             * @return int
             */
            int try_lock();

            /**
             * @brief unlock
             *
             * @return int
             */
            int unlock();

        private:
            pthread_mutex_t m_mutex;
        };
    }
}