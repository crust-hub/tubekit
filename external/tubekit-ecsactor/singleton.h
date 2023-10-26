#pragma once
#include <stdexcept>

namespace tubekit::ecsactor
{
    template <typename T>
    class singleton
    {
    public:
        template <typename... Args>
        static T *instance(Args &&...args)
        {
            if (m_ptr == nullptr)
            {
                m_ptr = new T(std::forward<Args>(args)...);
            }
            return m_ptr;
        }

        static T *get_instance()
        {
            if (m_ptr == nullptr)
            {
                throw std::logic_error("the instance is not init, please initialize the instance first");
            }
            return m_ptr;
        }

        static void destory_instance()
        {
            delete m_ptr;
            m_ptr = nullptr;
        }

    private:
        static T *m_ptr;
    };

    template <class T>
    T *singleton<T>::m_ptr = nullptr;
};
