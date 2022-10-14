#pragma once

#include <list>

#include "thread/task.h"
#include "thread/thread.h"
#include "log/logger.h"
#include "utility/singleton_template.h"
#include "thread/work_thread.h"
#include "thread/thread_pool.h"

using namespace tubekit::utility;
using namespace tubekit::thread;
using namespace tubekit::log;

namespace tubekit
{
    namespace thread
    {
        template <typename THREAD, typename TASK>
        class task_dispatcher : public thread
        {
        public:
            task_dispatcher();
            ~task_dispatcher();
            void init(size_t threads);
            void assign(TASK *task);
            void handle(TASK *taks);
            virtual void run();

        protected:
            std::list<TASK *> m_tasks;
        };

        template <typename THREAD, typename TASK>
        task_dispatcher<THREAD, TASK>::task_dispatcher()
        {
        }

        template <typename THREAD, typename TASK>
        task_dispatcher<THREAD, TASK>::~task_dispatcher()
        {
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::init(size_t threads)
        {
            singleton_template<thread_pool<THREAD, TASK>>::instance()->create(threads);
            singleton_template<logger>::instance()->debug(__FILE__, __LINE__, "threads create allrigth to check assign");
            start(); //是所有线程分离运行
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::assign(TASK *m_task)
        {
            singleton_template<logger>::instance()->debug(__FILE__, __LINE__, "task dispatcher assign task");
            m_mutex.lock();
            m_tasks.push_back(m_task);
            m_mutex.unlock();
            m_cond.signal(); // task coming
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::handle(TASK *m_task)
        {
            singleton_template<logger>::instance()->debug(__FILE__, __LINE__, "task dispatcher handle task");
            thread_pool<THREAD, TASK> *pool = singleton_template<thread_pool<THREAD, TASK>>::instance();
            if (pool->get_idle_thread_numbers() > 0)
            {
                pool->assign(m_task);
            }
            else
            {
                m_mutex.lock();
                m_tasks.push_front(m_task); //没有空余线程，则先放入列表
                m_mutex.unlock();
                singleton_template<logger>::instance()->debug(__FILE__, __LINE__, "all threads are busy");
            }
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::run()
        {
            sigset_t mask;
            if (0 != sigfillset(&mask))
            {
                singleton_template<logger>::instance()->error(__FILE__, __LINE__, "thread manager sigfillset failed");
                return;
            }
            if (0 != pthread_sigmask(SIG_SETMASK, &mask, nullptr))
            {
                singleton_template<logger>::instance()->error(__FILE__, __LINE__, "thread manager pthread_sigmask failed");
                return;
            } //只在主线程内处理信号

            while (true)
            {
                m_mutex.lock();
                while (m_tasks.empty())
                {
                    m_cond.wait(&m_mutex);
                }
                TASK *task = m_tasks.front();
                m_tasks.pop_front();
                m_mutex.unlock();
                handle(task);
            }
        }
    }
}
