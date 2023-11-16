#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <http-parser/http_parser.h>
#include <tubekit-buffer/buffer.h>

#include "connection/connection.h"
#include "socket/socket.h"

namespace tubekit
{
    namespace connection
    {
        class websocket_connection : public connection
        {
        public:
            websocket_connection(tubekit::socket::socket *socket_ptr);
            ~websocket_connection();

        public:
            virtual void on_mark_close() override;

        public:
            std::string url;
            std::map<std::string, std::vector<std::string>> headers;
            std::function<void(websocket_connection &connection)> process_callback;
            std::function<void(websocket_connection &connection)> write_end_callback;
            std::function<void(websocket_connection &connection)> destory_callback;
            void *ptr{nullptr};

        private:
            http_parser m_http_parser;
            bool connected{false};
        };
    }
}