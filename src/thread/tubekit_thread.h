#pragma once
#include <pthread.h>
#include "thread/mutex.h"
#include "thread/task.h"
#include "thread/condition.h"

namespace tubekit
{
    namespace thread
    {
        class tubekit_thread
        {
        public:
            tubekit_thread();
            virtual ~tubekit_thread();
            virtual void run() = 0;
            void start();
            void stop();
            void set_task(task *m_task);
            task *get_task();

        protected:
            /**
             * @brief run thread
             *
             * @param ptr
             * @return void*
             */
            static void *thread_func(void *ptr);

        protected:
            pthread_t m_tid;
            task *m_task;
            mutex m_mutex;
            condition m_cond;
        };
    }
}