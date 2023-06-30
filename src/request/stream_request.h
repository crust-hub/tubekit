#pragma once

#include <tubekit-buffer/buffer.h>

namespace tubekit
{
    namespace request
    {
        class stream_request
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
            stream_request(int socket_fd);
            ~stream_request();

        public:
            buffer::buffer m_send_buffer;
            buffer::buffer m_recv_buffer;
            state request_state;

        private:
            const int socket_fd;
        };
    }
}