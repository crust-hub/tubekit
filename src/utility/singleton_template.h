#pragma once
#include <memory>
namespace tubekit
{
    namespace utility
    {
        using namespace std;

        /**
         * @brief singleton template
         *
         * @tparam T
         */
        template <typename T>
        class singleton_template
        {
        public:
            static T *instance()
            {
                static shared_ptr<T> instance = nullptr;
                if (instance == nullptr)
                {
                    instance = make_shared<T>();
                }
                return instance.get();
            }

        protected:
            singleton_template() = default;
            ~singleton_template() = default;
        };
    }
}
