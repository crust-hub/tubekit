#pragma once
#include <string>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>

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
            bool init(const std::string &ip, int port, int max_connections, int wait_time);

            /**
             * @brief Register the m_socket with the epoll
             *
             * @param m_socket
             * @param listen_send true: listen EPOLLOUT|EPOLLIN false: listen EPOLLIN
             * @return int
             */
            int attach(socket *m_socket, bool listen_send = false);

            /**
             * @brief Remove from epoll
             *
             * @param m_socket
             */
            int detach(socket *m_socket);

            socket *alloc_socket();

            void handle();

        private:
            /**
             * @brief Remove from epoll and close the real socket and return it to the object pool
             *
             * @param m_socket
             */
            int remove(socket *m_socket);

        public:
            void on_tick();

        private:
            bool m_init;
            int m_max_connections;
            int m_wait_time;
            event_poller *m_epoll;
            socket *m_server;
            tubekit::thread::mutex m_mutex;
            SSL_CTX *m_ssl_context{nullptr};
        };
    }
}