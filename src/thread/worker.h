#pragma once
#include <pthread.h>
#include <signal.h>

#include "thread/thread.h"
#include "thread/task_queue.h"
#include "thread/task.h"
#include "thread/task_destory.h"

namespace tubekit::thread
{
    class worker : public thread
    {
    public:
        worker(task_destory *destory_ptr);
        virtual ~worker();
        virtual void run();

    public:
        void push(task *task_ptr);
        void stop();

    public:
        static void cleanup(void *ptr);

    private:
        task_queue m_task_queue;
        task_destory *m_destory_ptr{nullptr};
        volatile bool m_stoped{true};
    };
}
