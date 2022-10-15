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
            virtual void run();
            static void cleanup(void *ptr);
        };
    }
}