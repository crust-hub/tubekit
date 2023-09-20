#pragma once
#include "thread/mutex.h"

namespace tubekit
{
    namespace thread
    {
        class task
        {
        public:
            task();
            task(void *data);
            virtual ~task();

            void *get_data();
            void set_data(void *data);

            virtual void run() = 0;     // pure virtual function
            virtual void destroy() = 0; // interface
            virtual bool compare(task *other);

        protected:
            void *m_data;
        };
    }
}