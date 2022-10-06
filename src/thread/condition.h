#pragma once
#include <pthread.h>

#include "thread/mutex.h"

namespace tubekit
{
    namespace thread
    {
        /**
         * @brief condition variable
         *
         */
        class condition
        {
        public:
            condition();
            ~condition();

            /**
             * @brief unlock m_mutex to waiting status,trylock m_mutex when receive signal
             *
             * @param m_mutex should be locked before sending to this function
             * @return int
             */
            int wait(mutex *m_mutex);

            /**
             * @brief wakeup least one thread
             *
             * @return int
             */
            int signal();

            /**
             * @brief wakeup all thread
             *
             * @return int
             */
            int broadcast();

        protected:
            pthread_cond_t m_cond;
        };
    }
}