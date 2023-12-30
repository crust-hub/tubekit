#pragma once
#include "thread/task_destory.h"

namespace tubekit
{
    namespace task
    {
        class task_destory_impl :public tubekit::thread::task_destory
        {
        public:
            task_destory_impl();
            ~task_destory_impl();
            virtual void execute(tubekit::thread::task *task_ptr);
        };
    }
}
