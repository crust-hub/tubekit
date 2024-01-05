#pragma once
#include <stdexcept>
#include <memory>

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
                m_ptr.reset(new T(std::forward<Args>(args)...));
            }
            return m_ptr.get();
        }

        static T *get_instance()
        {
            if (m_ptr == nullptr)
            {
                throw std::logic_error("the instance is not init, please initialize the instance first");
            }
            return m_ptr.get();
        }

        static void destory_instance()
        {
            m_ptr.release();
            m_ptr.reset(nullptr);
        }

    private:
        static std::unique_ptr<T> m_ptr;
    };

    template <class T>
    std::unique_ptr<T> singleton<T>::m_ptr = nullptr;
};
