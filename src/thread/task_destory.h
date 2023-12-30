#pragma once
#include "thread/task.h"

namespace tubekit
{
    namespace thread
    {
        class task_destory
        {
        public:
            task_destory();
            virtual ~task_destory();

            virtual void execute(task *task_ptr) = 0;
        };
    }
}