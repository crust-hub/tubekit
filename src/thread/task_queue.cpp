#include "thread/task_queue.h"

using tubekit::thread::task;
using tubekit::thread::task_queue;

task_queue::task_queue()
{
}

task_queue::~task_queue()
{
}

bool task_queue::push(task *task_ptr)
{
    auto_lock lock(m_mutex);

    if (nullptr == task_ptr)
    {
        m_task.push_back(task_ptr);
        m_condition.broadcast();
        return true;
    }

    auto iter = m_in_task.find(task_ptr->get_gid());
    if (iter != m_in_task.end())
    {
        m_condition.broadcast();
        return false;
    }

    m_task.push_back(task_ptr);
    m_in_task.insert(task_ptr->get_gid());
    m_condition.broadcast();

    return true;
}

task *task_queue::pop()
{
    auto_lock lock(m_mutex);
    while (m_task.empty())
    {
        m_condition.wait(&m_mutex);
    }
    task *task_ptr = m_task.front();
    m_task.pop_front();

    if (task_ptr)
    {
        auto iter = m_in_task.find(task_ptr->get_gid());
        if (iter != m_in_task.end())
        {
            m_in_task.erase(iter);
        }
    }

    return task_ptr;
}
