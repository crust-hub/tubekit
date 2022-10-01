#pragma once
#include <pthread.h>
#include <signal.h>
#include "thread/thread.h"

namespace tubekit
{
    namespace thread
    {
        class work_thread : public thread
        {
        public:
            work_thread();
            virtual ~work_thread();

            //// run将会在thread的start方法调用时以线程分离的形式执行
            virtual void run();
            static void cleanup(void *ptr);
        };
    }
}