#pragma once

#include "thread/task.h"
#include "socket/socket.h"

namespace tubekit
{
    namespace task
    {
        class stream_task : public tubekit::thread::task
        {
        public:
            stream_task(tubekit::socket::socket *m_socket);
            ~stream_task();
            void run();
            /**
             * @brief Manual destruction,execute in ~stream_task
             *
             */
            void destroy();

        public:
            // flag:why create task
            bool reason_recv{false};
            bool reason_send{false};
        };
    }
}