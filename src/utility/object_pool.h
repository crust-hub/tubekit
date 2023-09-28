#pragma once
#include <set>

#include "thread/mutex.h"
#include "thread/auto_lock.h"
#include "thread/condition.h"

namespace tubekit
{
    namespace utility
    {
        using namespace thread;
        template <typename T>
        class object_pool
        {
        public:
            object_pool() = default;
            ~object_pool();

            /**
             * @brief init object to storage in list
             *
             * @param max_size number of object
             */
            void init(size_t max_size);

            /**
             * @brief get a object from list
             *
             * @return T* can null
             */
            T *allocate();

            /**
             * @brief return object to list
             *
             */
            void release(T *t);

        private:
            std::set<T *> m_set;

            /**
             * @brief ensure thread safety for m_list operations
             *
             */
            mutex m_mutex;
            condition m_condition;
        };

        template <typename T>
        object_pool<T>::~object_pool()
        {
            auto_lock lock(m_mutex);
            // free space of the all objects
            for (auto t : m_set)
            {
                delete t;
            }
            m_set.clear();
        }

        template <typename T>
        void object_pool<T>::init(size_t max_size)
        {
            auto_lock lock(m_mutex);
            for (size_t i = 0; i < max_size; ++i)
            {
                T *p = new T();
                if (p)
                    m_set.insert(p);
            }
        }

        template <typename T>
        T *object_pool<T>::allocate()
        {
            auto_lock lock(m_mutex);
            while (m_set.empty())
            {
                m_condition.wait(&m_mutex);
            }
            T *p = *m_set.begin();
            m_set.erase(m_set.begin());
            return p;
        }

        template <typename T>
        void object_pool<T>::release(T *t)
        {
            if (t)
            {
                m_mutex.lock();
                m_set.insert(t);
                m_mutex.unlock();
                m_condition.broadcast();
            }
        }

    }
}