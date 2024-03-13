#pragma once

#include <http-parser/http_parser.h>

#include "thread/task.h"
#include "socket/socket.h"
#include "connection/http_connection.h"

namespace tubekit
{
    namespace task
    {
        class http_task : public tubekit::thread::task
        {
        public:
            http_task(tubekit::socket::socket *m_socket);
            ~http_task();
            void run() override;
            /**
             * @brief Manual destruction,execute in ~http_task()
             *
             */
            void destroy() override;

        public:
            static http_parser_settings *settings;
            // flag:why create task
            bool reason_recv{false};
            bool reason_send{false};
        };
    }
}