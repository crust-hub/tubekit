#include <list>
#include <unordered_set>
#include "thread/mutex.h"
#include "thread/condition.h"
#include "thread/auto_lock.h"
#include "thread/task.h"

namespace tubekit::thread
{
    class task_queue
    {
    public:
        task_queue();
        ~task_queue();
        bool push(task *task_ptr);
        task *pop();

    private:
        mutex m_mutex;
        condition m_condition;
        std::list<task *> m_task{};
        std::unordered_set<void *> m_in_task{};
    };
}
