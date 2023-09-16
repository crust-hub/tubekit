#pragma once
#include <memory>

namespace tubekit
{
    namespace utility
    {
        /**
         * @brief singleton template
         *
         * @tparam T
         */
        template <typename T>
        class singleton
        {
        public:
            static T *instance()
            {
                static std::shared_ptr<T> instance = nullptr;
                if (instance == nullptr)
                {
                    instance = std::make_shared<T>();
                }
                return instance.get();
            }

        protected:
            singleton() = default;
            ~singleton() = default;
        };
    }
}
