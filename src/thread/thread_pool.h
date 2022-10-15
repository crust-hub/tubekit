#pragma once

#include <set>

#include "thread/mutex.h"
#include "thread/condition.h"
#include "thread/thread_pool.h"
#include "log/logger.h"
#include "utility/singleton_template.h"
#include "thread/auto_lock.h"

using namespace tubekit::utility;
using namespace tubekit::thread;
using namespace tubekit::log;

namespace tubekit
{
    namespace thread
    {
        /**
         * @brief
         *
         * @tparam THREAD must be having set_task(TASK*task) method
         * @tparam TASK
         */
        template <typename THREAD, typename TASK>
        class thread_pool
        {
        public:
            thread_pool();
            ~thread_pool();
            /**
             * @brief
             *
             * @param number of threads
             */
            void create(size_t threads);

            /**
             * @brief Get the idle thread object
             *
             * @return thread*
             */
            THREAD *get_idle_thread();

            void move_to_idle_list(THREAD *m_thread);
            void move_to_busy_list(THREAD *m_thread);

            size_t get_idle_thread_numbers();
            size_t get_busy_thread_numbers();

            /**
             * @brief dispatch a task for processing it
             *
             * @param m_task
             */
            void assign(TASK *m_task);

        private:
            /**
             * @brief number of threads in pool
             *
             */
            size_t m_threads;

            // using set,prevent having same thread in same list
            std::set<THREAD *> m_list_idle;
            std::set<THREAD *> m_list_busy;

            mutex m_mutex_idle;
            mutex m_mutex_busy;

            condition m_cond_idle;
            condition m_cond_busy;
        };

        template <typename THREAD, typename TASK>
        thread_pool<THREAD, TASK>::thread_pool() : m_threads(0)
        {
        }

        template <typename THREAD, typename TASK>
        thread_pool<THREAD, TASK>::~thread_pool()
        {
        }

        template <typename THREAD, typename TASK>
        void thread_pool<THREAD, TASK>::create(size_t threads)
        {
            auto_lock lock(m_mutex_idle);
            m_threads = threads;
            for (size_t i = 0; i < threads; i++)
            {
                THREAD *new_thread = new THREAD();
                singleton_template<logger>::instance()->debug(__FILE__, __LINE__, "create thread %x", new_thread);
                m_list_idle.insert(new_thread);
                new_thread->start();
            }
        }

        template <typename THREAD, typename TASK>
        THREAD *thread_pool<THREAD, TASK>::get_idle_thread()
        {
            auto_lock lock(m_mutex_idle);
            while (m_list_idle.size() == 0)
            {
                m_cond_idle.wait(&m_mutex_idle);
            }
            return *m_list_idle.begin();
        }

        /**
         * @brief Feed idle threads to the thread pool
         *
         * @tparam THREAD
         * @tparam TASK
         * @param m_thread
         */
        template <typename THREAD, typename TASK>
        void thread_pool<THREAD, TASK>::move_to_idle_list(THREAD *m_thread)
        {
            m_mutex_idle.lock();
            m_list_idle.insert(m_thread);
            m_cond_idle.signal();
            m_mutex_idle.unlock();

            m_mutex_busy.lock();
            auto res = m_list_busy.find(m_thread);
            if (res != m_list_busy.end())
            {
                m_list_busy.erase(res);
            }
            m_cond_busy.signal();
            m_mutex_busy.unlock();
        }

        template <typename THREAD, typename TASK>
        void thread_pool<THREAD, TASK>::move_to_busy_list(THREAD *m_thread)
        {
            // busy'size dose not over max litmit
            m_mutex_busy.lock();
            while (m_list_busy.size() == m_threads)
            {
                m_cond_busy.wait(&m_mutex_busy);
            }
            m_list_busy.insert(m_thread);
            m_mutex_busy.unlock();

            m_mutex_idle.lock();
            auto res = m_list_idle.find(m_thread);
            if (res != m_list_idle.end())
            {
                m_list_idle.erase(res);
            }
            m_mutex_idle.unlock();
        }

        template <typename THREAD, typename TASK>
        size_t thread_pool<THREAD, TASK>::get_busy_thread_numbers()
        {
            auto_lock lock(m_mutex_busy);
            return m_list_busy.size();
        }

        template <typename THREAD, typename TASK>
        size_t thread_pool<THREAD, TASK>::get_idle_thread_numbers()
        {
            auto_lock lock(m_mutex_idle);
            return m_list_idle.size();
        }

        template <typename THREAD, typename TASK>
        void thread_pool<THREAD, TASK>::assign(TASK *m_task)
        {
            if (m_task == nullptr)
            {
                singleton_template<logger>::instance()->error(__FILE__, __LINE__, "assign a null task to thread pool");
                return;
            }
            singleton_template<logger>::instance()->debug(__FILE__, __LINE__, "assign a new task %x to thread pool", m_task);

            m_mutex_busy.lock();

            // all busy state
            while (m_list_busy.size() == m_threads)
            {
                m_cond_busy.wait(&m_mutex_busy);
            }
            m_mutex_busy.unlock();

            THREAD *idle_thread = get_idle_thread();
            if (idle_thread != nullptr)
            {
                move_to_busy_list(idle_thread);

                idle_thread->set_task(m_task);
            }
            else
            {
                singleton_template<logger>::instance()->error(__FILE__, __LINE__, "thread is null,assign a task failed");
            }
        }
    }
}