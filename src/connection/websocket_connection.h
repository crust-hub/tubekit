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
            http_parser *get_parser();
            void add_header(const std::string &key, const std::string &value);

        public:
            virtual void on_mark_close() override;
            virtual void reuse() override;

        private:
            bool sock2buf(bool &need_task);
            bool buf2sock(bool &closed);

        public:
            inline bool get_connected()
            {
                return connected;
            }

            inline void set_connected(bool connected)
            {
                this->connected = connected;
            }

            /**
             * @brief Only when processing websocket_connection, the worker thread of the connection uses its own send,
             *        and if sending to other connections, the connection mgr's safe_send needs to be used.
             *
             * @param buffer
             * @param buffer_size
             * @param check_connected
             * @return true
             * @return false
             */
            bool send(const char *buffer, size_t buffer_size, bool check_connected = true);

        public:
            std::string url{};
            std::string method{};
            std::string sec_websocket_key{};
            std::string sec_websocket_version{};
            std::map<std::string, std::vector<std::string>> headers{};
            const size_t buffer_size{20230};
            char buffer[20230]{0};
            int buffer_used_len{0};
            int buffer_start_use{0};
            std::string head_filed_tmp{};
            bool http_processed{false};
            bool everything_end{false};
            bool is_upgrade{false};
            buffer::buffer m_recv_buffer;
            buffer::buffer m_send_buffer;
            buffer::buffer m_wating_send_pack;

            std::function<void(websocket_connection &connection)> destory_callback;

        private:
            int should_send_idx{-1};
            int should_send_size{0};

        private:
            static constexpr size_t inner_buffer_size{1024000};
            char buf2sock_inner_buffer[1024000]{0};
            char sock2buf_inner_buffer[1024000]{0};

            int sock2buf_data_len{0};

            http_parser m_http_parser;
            bool connected{false};
        };
    }
}