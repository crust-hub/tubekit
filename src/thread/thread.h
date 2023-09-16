#pragma once
#include <pthread.h>

namespace tubekit
{
    namespace thread
    {
        class thread
        {
        public:
            thread();
            virtual ~thread();
            virtual void run() = 0;
            virtual void to_stop();

            /**
             * @brief Run the thread in a detach form and call the run method
             *
             */
            void start();

        protected:
            /**
             * @brief run thread
             *
             * @param ptr
             * @return void*
             */
            static void *thread_func(void *ptr);
            void stop();

        protected:
            pthread_t m_tid;
            bool stop_flag{false};
        };
    }
}