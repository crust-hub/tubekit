#include <queue>
#include "thread/mutex.h"
#include "thread/condition.h"
#include "thread/auto_lock.h"

namespace tubekit::thread
{
    template <typename TASK>
    class task_queue
    {
    public:
        task_queue();
        ~task_queue();
        void push(TASK *task);
        TASK *pop();

    private:
        mutex m_mutex;
        condition m_condition;
        std::queue<TASK *> m_task;
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
    void task_queue<TASK>::push(TASK *task)
    {
        auto_lock lock(m_mutex);
        m_task.push(task);
        m_condition.broadcast();
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
        m_task.pop();
        return task;
    }
}