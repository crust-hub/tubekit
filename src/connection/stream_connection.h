#pragma once

#include <tubekit-buffer/buffer.h>

#include "connection/connection.h"
#include "socket/socket.h"

namespace tubekit
{
    namespace connection
    {
        class stream_connection : public connection
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
            stream_connection(tubekit::socket::socket *socket_ptr);
            ~stream_connection();

        public:
            buffer::buffer m_send_buffer;
            buffer::buffer m_recv_buffer;
            state connection_state;

        private:
            tubekit::socket::socket *socket_ptr;
        };
    }
}