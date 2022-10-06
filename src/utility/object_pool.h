#pragma once
#include <list>

#include "thread/mutex.h"
#include "thread/auto_lock.h"

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
            std::list<T *> m_list;

            /**
             * @brief ensure thread safety for m_list operations
             *
             */
            mutex m_mutex;
        };

        template <typename T>
        object_pool<T>::~object_pool()
        {
            auto_lock lock(m_mutex);
            // free space of the all objects
            for (auto t : m_list)
            {
                delete t;
            }
            m_list.clear();
        }

        template <typename T>
        void object_pool<T>::init(size_t max_size)
        {
            auto_lock lock(m_mutex);
            for (size_t i = 0; i < max_size; ++i)
            {
                T *p = new T();
                if (p)
                    m_list.push_back(p);
            }
        }

        template <typename T>
        T *object_pool<T>::allocate()
        {
            auto_lock lock(m_mutex);
            if (0 == m_list.size())
            {
                return nullptr;
            }
            T *p = m_list.front(); // head node
            m_list.pop_front();    // remove head node
            return p;
        }

        template <typename T>
        void object_pool<T>::release(T *t)
        {
            auto_lock lock(m_mutex);
            if (t)
                m_list.push_back(t);
        }

    }
}