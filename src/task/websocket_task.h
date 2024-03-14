#pragma once

#include <http-parser/http_parser.h>

#include "thread/task.h"
#include "socket/socket.h"

namespace tubekit
{
    namespace task
    {
        class websocket_task : public tubekit::thread::task
        {
        public:
            websocket_task(uint64_t gid);
            ~websocket_task();
            void run() override;
            void destroy() override;

        public:
            static http_parser_settings *settings;
        };
    }
}