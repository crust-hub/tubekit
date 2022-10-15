#pragma once

#include "thread/task.h"
#include "socket/socket.h"

namespace tubekit
{
    namespace task
    {
        // Protocol header format
        struct work_task_msg_head
        {
            char flag[8];
            uint32_t cmd;
            uint32_t len;
        };

        // buffer size
        constexpr uint32_t recv_buffer_size = 1024;

        class work_task : public tubekit::thread::task
        {
        public:
            work_task(tubekit::socket::socket *m_socket);
            ~work_task();
            void run();
            /**
             * @brief Manual destruction
             *
             */
            void destroy();
        };
    }
}