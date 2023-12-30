#include "task/task_destory_impl.h"
#include "task/task_mgr.h"
#include "utility/singleton.h"

using tubekit::task::task_destory_impl;
using tubekit::task::task_mgr;
using tubekit::utility::singleton;

task_destory_impl::task_destory_impl()
{
}

task_destory_impl::~task_destory_impl()
{
}

void task_destory_impl::execute(tubekit::thread::task *task_ptr)
{
    if (task_ptr)
    {
        singleton<task_mgr>::instance()->release(task_ptr);
    }
}