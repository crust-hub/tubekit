#pragma once
#include <string>

#include "./socket.h"
#include "./event_poller.h"
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
             * @brief 将m_socket注册到epoll中
             *
             * @param m_socket
             */
            void attach(socket *m_socket);
            /**
             * @brief 从epoll中移除
             *
             * @param m_socket
             */
            void detach(socket *m_socket);
            /**
             * @brief 关闭m_socket并将其返回对象池
             *
             * @param m_socket
             */
            void remove(socket *m_socket);
            void handle(int max_connections, int wait_time);

        private:
            event_poller *m_epoll;
            socket *m_server;
            tubekit::utility::object_pool<socket> socket_pool;
            tubekit::thread::mutex m_mutex;
        };
    }
}