#include <list>
#include "thread/mutex.h"
#include "thread/condition.h"
#include "thread/auto_lock.h"
#include "task/task_mgr.h"
#include <tubekit-log/logger.h>

namespace tubekit::thread
{
    template <typename TASK>
    class task_queue
    {
    public:
        task_queue();
        ~task_queue();
        bool push(TASK *task);
        TASK *pop();

    private:
        mutex m_mutex;
        condition m_condition;
        std::list<TASK *> m_task;
    };

    template <typename TASK>
    task_queue<TASK>::task_queue()
    {
    }

    template <typename TASK>
    task_queue<TASK>::~task_queue()
    {
    }

    template <typename TASK>
    bool task_queue<TASK>::push(TASK *task)
    {
        auto_lock lock(m_mutex);
        for (auto ptr : m_task)
        {
            if (ptr && ptr->compare(task))
            {
                return false;
            }
        }
        m_task.push_back(task);
        m_condition.broadcast();
        return true;
    }

    template <typename TASK>
    TASK *task_queue<TASK>::pop()
    {
        auto_lock lock(m_mutex);
        while (m_task.empty())
        {
            m_condition.wait(&m_mutex);
        }
        TASK *task = m_task.front();
        m_task.pop_front();
        return task;
    }
}
