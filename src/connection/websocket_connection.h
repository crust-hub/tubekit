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
#include "task/websocket_task.h"

namespace tubekit
{
    namespace connection
    {
        class websocket_connection : public connection
        {
        public:
            friend class tubekit::task::websocket_task;

        public:
            websocket_connection(tubekit::socket::socket *socket_ptr);
            ~websocket_connection();

        public:
            virtual void on_mark_close() override;
            http_parser *get_parser();
            void add_header(const std::string &key, const std::string &value);

        private:
            bool sock2buf();
            bool buf2sock();

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
            tubekit::buffer::buffer m_send_buffer;
            buffer::buffer m_wating_send_pack;

            std::function<void(websocket_connection &connection)> destory_callback;
            void *ptr{nullptr};

        public:
            bool get_connected();
            void set_connected(bool connected);
            /**
             * @brief Only when processing websocket_connection, the worker thread of the connection uses its own send,
             *        and if sending to other connections, the connection mgr's safe_send needs to be used.
             *
             * @param buffer
             * @param buffer_size
             * @return true
             * @return false
             */
            bool send(const char *buffer, size_t buffer_size);

        private:
            http_parser m_http_parser;
            bool connected{false};
        };
    }
}