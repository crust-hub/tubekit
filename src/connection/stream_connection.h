#pragma once

#include <tubekit-buffer/buffer.h>

namespace tubekit
{
    namespace connection
    {
        class stream_connection
        {
        public:
            enum class state
            {
                RECV,
                SEND,
                PROCESS,
                NONE
            };

        public:
            stream_connection(int socket_fd);
            ~stream_connection();

        public:
            buffer::buffer m_send_buffer;
            buffer::buffer m_recv_buffer;
            state connection_state;

        private:
            const int socket_fd;
        };
    }
}