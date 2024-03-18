#pragma once

#include <tubekit-buffer/buffer.h>

#include "connection/connection.h"
#include "socket/socket.h"
#include "task/stream_task.h"

namespace tubekit
{
    namespace connection
    {
        class stream_connection : public connection
        {
        public:
            friend class tubekit::task::stream_task;

        public:
            stream_connection(tubekit::socket::socket *socket_ptr);
            ~stream_connection();

        private:
            bool sock2buf(bool &need_task);
            bool buf2sock(bool &closed);

        public:
            /**
             * @brief Only when processing stream_connection, the worker thread of the connection uses its own send,
             *        and if sending to other connections, the connection mgr's safe_send needs to be used.
             *
             * @param buffer
             * @param buffer_size
             * @return true
             * @return false
             */
            bool send(const char *buffer, size_t buffer_size);

        public:
            virtual void on_mark_close() override;
            virtual void reuse() override;

        public:
            buffer::buffer m_send_buffer;
            buffer::buffer m_recv_buffer;
            buffer::buffer m_wating_send_pack;

        private:
            int should_send_idx{-1};
            int should_send_size{0};
            char buf2sock_inner_buffer[1024]{0};
            char sock2buf_inner_buffer[1024]{0};
            const size_t inner_buffer_size{1024};
            int sock2buf_data_len{0};
        };
    }
}