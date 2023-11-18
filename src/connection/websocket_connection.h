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
            http_parser *get_parser();
            void add_header(const std::string &key, const std::string &value);

        public:
            std::string url;
            std::string method;
            std::string sec_websocket_key;
            std::string sec_websocket_version;
            std::map<std::string, std::vector<std::string>> headers;
            const size_t buffer_size{202300};
            char buffer[202300];
            int buffer_used_len;
            int buffer_start_use;
            std::string head_filed_tmp;
            bool http_processed{false};
            bool everything_end{false};
            bool is_upgrade{false};
            tubekit::buffer::buffer m_recv_buffer;

            std::function<void(websocket_connection &connection)> process_callback;
            std::function<void(websocket_connection &connection)> write_end_callback;
            std::function<void(websocket_connection &connection)> destory_callback;
            void *ptr{nullptr};

        public:
            bool get_connected();
            void set_connected(bool connected);

        private:
            http_parser m_http_parser;
            bool connected{false};
        };
    }
}