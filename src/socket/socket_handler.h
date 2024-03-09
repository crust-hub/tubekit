#pragma once
#include <string>

#include "socket/socket.h"
#include "socket/event_poller.h"
#include "utility/object_pool.h"
#include "thread/mutex.h"
#include <list>
#include <set>

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

            /**
             * @brief Remove from epoll and close the real socket and return it to the object pool
             *
             * @param m_socket
             */
            int remove(socket *m_socket);

            void do_task(socket *m_socket, bool recv_event, bool send_event);

        public:
            void push_wait_remove(socket *m_socket);
            void update_wait_remove(std::set<socket *> &removed_socket);

        public:
            void on_tick();

        private:
            bool m_init;
            int m_max_connections;
            int m_wait_time;
            event_poller *m_epoll;
            socket *m_server;
            tubekit::thread::mutex m_mutex;

            tubekit::thread::mutex m_remove_mutex;
            std::list<socket *> m_remove_list1;
            std::list<socket *> m_remove_list2;
            std::list<socket *> *m_read_remove_list;
            std::list<socket *> *m_write_remove_list;
        };
    }
}