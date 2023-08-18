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
            LOG_DEBUG("threads create allrigth to check assign");
            start(); // task_dispatcher::run start with other thread
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::assign(TASK *m_task)
        {
            // LOG_DEBUG("task dispatcher assign task");
            m_mutex.lock();
            m_tasks.push_back(m_task);
            m_mutex.unlock();
            m_cond.signal(); // task coming
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::handle(TASK *m_task)
        {
            // LOG_DEBUG("task dispatcher handle task");
            thread_pool<THREAD, TASK> *pool = singleton<thread_pool<THREAD, TASK>>::instance();
            pool->assign(m_task);
        }

        template <typename THREAD, typename TASK>
        void task_dispatcher<THREAD, TASK>::run()
        {
            sigset_t mask;
            if (0 != sigfillset(&mask))
            {
                LOG_ERROR("thread manager sigfillset failed");
                return;
            }
            if (0 != pthread_sigmask(SIG_SETMASK, &mask, nullptr))
            {
                LOG_ERROR("thread manager pthread_sigmask failed");
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
                if (stop_flag)
                {
                    break;
                }
                TASK *task = m_tasks.front();
                m_tasks.pop_front();
                m_mutex.unlock();
                handle(task);
            }
        }
    }
}
