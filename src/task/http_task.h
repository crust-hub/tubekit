#pragma once

#include "thread/task.h"
#include "socket/socket.h"
#include "http/http_session.h"

namespace tubekit
{
    namespace task
    {
        class http_task : public tubekit::thread::task
        {
        public:
            http_task(tubekit::socket::socket *m_socket);
            ~http_task();
            void run();
            /**
             * @brief Manual destruction
             *
             */
            void destroy();
            static void test(const http::session *m_session);
        };
    }
}