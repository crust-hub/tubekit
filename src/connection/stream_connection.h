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
            bool sock2buf();
            bool buf2sock();

        // public:
        //     bool send(char *buffer, size_t buffer_size);

        public:
            buffer::buffer m_send_buffer;
            buffer::buffer m_recv_buffer;
            buffer::buffer m_wating_send_pack;

        private:
            tubekit::socket::socket *socket_ptr;
        };
    }
}