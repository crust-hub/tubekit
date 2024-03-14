#pragma once

#include "thread/task.h"
#include "socket/socket.h"
#include "task/task_type.h"
#include "thread/task.h"

namespace tubekit
{
    namespace task
    {
        class task_mgr
        {
        public:
            task_mgr();
            int init(tubekit::task::task_type task_type);
            tubekit::thread::task *create(uint64_t gid);
            void release(tubekit::thread::task *task_ptr);

        private:
            tubekit::task::task_type m_task_type{tubekit::task::task_type::NONE};
        };
    }
}
