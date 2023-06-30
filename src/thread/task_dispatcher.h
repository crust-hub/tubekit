#pragma once

#include <list>
#include <tubekit-log/logger.h>

#include "thread/task.h"
#include "thread/thread.h"
#include "utility/singleton.h"
#include "thread/work_thread.h"
#include "thread/thread_pool.h"

using namespace tubekit::utility;
using namespace tubekit::thread;
using namespace tubekit::log;

namespace tubekit
{
    namespace thread
    {
        /**
         * @brief use THREAD to process TASK
         *
         * @tparam THREAD
         * @tparam TASK
         */
        template <typename THREAD, typename TASK>
        class task_dispatcher : public thread
        {
        public:
            task_dispatcher();
            ~task_dispatcher();
            /**
             * @brief init threads
             *
             * @param threads
             */
            void init(size_t threads);
            /**
             * @brief add to m_tasks
             *
             * @param task
             */
            void assign(TASK *task);
            /**
             * @brief send to thread pool
             *
             * @param task
             */
            void handle(TASK *task);
            /**
             * @brief task_dispatcher thread start
             *
             */
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
            singleton<thread_pool<THREAD, TASK>>::instance()->create(threads);
            singleton<logger>::instance()->debug(__FILE__, __LINE__, "threads create allrigth to check assign");
            start(); // task_dispatcher::run start with other thread
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::assign(TASK *m_task)
        {
            singleton<logger>::instance()->debug(__FILE__, __LINE__, "task dispatcher assign task");
            m_mutex.lock();
            m_tasks.push_back(m_task);
            m_mutex.unlock();
            m_cond.signal(); // task coming
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::handle(TASK *m_task)
        {
            singleton<logger>::instance()->debug(__FILE__, __LINE__, "task dispatcher handle task");
            thread_pool<THREAD, TASK> *pool = singleton<thread_pool<THREAD, TASK>>::instance();
            if (pool->get_idle_thread_numbers() > 0)
            {
                pool->assign(m_task);
            }
            else
            {
                m_mutex.lock();
                m_tasks.push_front(m_task); // If there are no free threads, they are put into the list first
                m_mutex.unlock();
                singleton<logger>::instance()->debug(__FILE__, __LINE__, "all threads are busy");
            }
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::run()
        {
            sigset_t mask;
            if (0 != sigfillset(&mask))
            {
                singleton<logger>::instance()->error(__FILE__, __LINE__, "thread manager sigfillset failed");
                return;
            }
            if (0 != pthread_sigmask(SIG_SETMASK, &mask, nullptr))
            {
                singleton<logger>::instance()->error(__FILE__, __LINE__, "thread manager pthread_sigmask failed");
                return;
            } // Signals are processed only within the main thread

            while (true)
            {
                m_mutex.lock();
                while (m_tasks.empty())
                {
                    // to wait if have not task in m_tasks
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
