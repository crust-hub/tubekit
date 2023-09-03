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
                WAIT_RECV,
                RECVING,
                RECVED,
                WAIT_SEND,
                SENDING,
                WAIT_PROCESS,
                PROCESSING,
                WAIT_DISCONNECT,
                CLOSED,
                NONE
            };

        public:
            stream_connection(tubekit::socket::socket *socket_ptr);
            ~stream_connection();

        public:
            bool sock2buf();
            bool buf2sock();

        public:
            buffer::buffer m_send_buffer;
            buffer::buffer m_recv_buffer;
            state connection_state;

        private:
            tubekit::socket::socket *socket_ptr;
        };
    }
}