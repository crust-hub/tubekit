#pragma once

#include "thread/task.h"
#include "socket/socket.h"
#include "connection/websocket_connection.h"

namespace tubekit
{
    namespace task
    {
        class websocket_task : public tubekit::thread::task
        {
        public:
            websocket_task(tubekit::socket::socket *m_socket);
            ~websocket_task();
            void run() override;
            void destroy() override;
        };
    }
}