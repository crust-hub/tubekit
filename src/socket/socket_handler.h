#pragma once
#include <string>

#include "socket/socket.h"
#include "socket/event_poller.h"
#include "utility/object_pool.h"
#include "thread/mutex.h"

namespace tubekit
{
    namespace socket
    {
        class socket_handler
        {
        public:
            socket_handler();
            ~socket_handler();
            void listen(const std::string &ip, int port);
            /**
             * @brief Register the m_socket with the epoll
             *
             * @param m_socket
             */
            void attach(socket *m_socket, bool listen_read = false);
            /**
             * @brief Remove from epoll
             *
             * @param m_socket
             */
            void detach(socket *m_socket);
            /**
             * @brief Remove from epoll and close the real socket and return it to the object pool
             *
             * @param m_socket
             */
            void remove(socket *m_socket);
            void handle(int max_connections, int wait_time);

        public:
            void on_tick();

        private:
            event_poller *m_epoll;
            socket *m_server;
            tubekit::utility::object_pool<socket> socket_pool;
            tubekit::thread::mutex m_mutex;
        };
    }
}