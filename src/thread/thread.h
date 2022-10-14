#pragma once
#include <pthread.h>

#include "thread/mutex.h"
#include "thread/task.h"
#include "thread/condition.h"

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

            /**
             * @brief 以线程分离形式运行线程，调用run方法
             *
             */
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