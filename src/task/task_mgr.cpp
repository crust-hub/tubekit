#include "task/task_mgr.h"

#include "task/stream_task.h"
#include "task/http_task.h"
#include "task/websocket_task.h"
#include "utility/singleton.h"
#include "utility/object_pool.h"
#include <tubekit-log/logger.h>

using tubekit::task::task_mgr;

task_mgr::task_mgr()
{
}

int task_mgr::init(tubekit::task::task_type task_type)
{
    m_task_type = task_type;
    return 0;
}

tubekit::thread::task *task_mgr::create(uint64_t gid)
{
    tubekit::thread::task *task_ptr = nullptr;
    switch (m_task_type)
    {
    case STREAM_TASK:
    {
        task_ptr = tubekit::utility::singleton<tubekit::utility::object_pool<stream_task>>::instance()->allocate();
        break;
    }
    case HTTP_TASK:
    {
        task_ptr = tubekit::utility::singleton<tubekit::utility::object_pool<http_task>>::instance()->allocate();
        break;
    }
    case WEBSOCKET_TASK:
    {
        task_ptr = tubekit::utility::singleton<tubekit::utility::object_pool<websocket_task>>::instance()->allocate();
        break;
    }
    default:
        break;
    }
    if (task_ptr)
    {
        task_ptr->set_gid(gid);
    }
    return task_ptr;
}

void task_mgr::release(tubekit::thread::task *task_ptr)
{
    if (!task_ptr)
    {
        return;
    }
    switch (m_task_type)
    {
    case STREAM_TASK:
    {
        tubekit::utility::singleton<tubekit::utility::object_pool<stream_task>>::instance()->release(dynamic_cast<stream_task *>(task_ptr));
        // LOG_DEBUG("task space:%d", tubekit::utility::singleton<tubekit::utility::object_pool<stream_task>>::instance()->space());
        break;
    }
    case HTTP_TASK:
    {
        tubekit::utility::singleton<tubekit::utility::object_pool<http_task>>::instance()->release(dynamic_cast<http_task *>(task_ptr));
        // LOG_DEBUG("task space:%d", tubekit::utility::singleton<tubekit::utility::object_pool<http_task>>::instance()->space());
        break;
    }
    case WEBSOCKET_TASK:
    {
        tubekit::utility::singleton<tubekit::utility::object_pool<websocket_task>>::instance()->release(dynamic_cast<websocket_task *>(task_ptr));
        // LOG_DEBUG("task space:%d", tubekit::utility::singleton<tubekit::utility::object_pool<websocket_task>>::instance()->space());
        break;
    }
    default:
        break;
    }
}