#include "udp_component.h"

namespace tubekit
{
    namespace ipc
    {
        static int udp_component_setnonblocking(int fd)
        {
            int flags = fcntl(fd, F_GETFL, 0);
            if (flags == -1)
            {
                perror("error getting fd flags");
                return -1;
            }
            if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            {
                perror("error setting non-bloking mode");
                return -1;
            }
            return 0;
        }

        udp_component::~udp_component()
        {
            to_close();
        }

        void udp_component::to_close()
        {
            if (m_socket_fd > 0)
            {
                close(m_socket_fd);
            }
            if (m_epoll_fd > 0)
            {
                close(m_epoll_fd);
            }
            if (close_callback)
            {
                close_callback();
            }
        }

        int udp_component::init_sock()
        {
            int &server_socket_fd = m_socket_fd;
            if (server_socket_fd <= 0)
            {
                server_socket_fd = ::socket(AF_INET, SOCK_DGRAM, 0);
                if (server_socket_fd == -1)
                {
                    perror("error creating socket");
                    return -1;
                }
            }
            return 0;
        }

        std::string udp_component::udp_component_get_ip(struct sockaddr_in &addr)
        {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
            return std::string(ip, INET_ADDRSTRLEN);
        }

        int udp_component::udp_component_get_port(struct sockaddr_in &addr)
        {
            unsigned short port = ntohs(addr.sin_port);
            return port;
        }

        int udp_component::udp_component_server(const std::string &IP,
                                                const int PORT)
        {
            const int int_port = PORT;

            // create udp socket
            int &server_socket_fd = m_socket_fd;
            if (server_socket_fd <= 0)
            {
                if (0 != init_sock())
                {
                    perror("Error creating socket");
                    return -1;
                }
            }

            // setting server addr
            struct sockaddr_in server_addr;
            bzero(&server_addr, sizeof(server_addr));

            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr(IP.c_str());
            server_addr.sin_port = htons(int_port);

            // server bind addr
            if (-1 == bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
            {
                perror("error binding socket");
                to_close();
                return -1;
            }

            // port reuse
            int reuse = 1;
            if (-1 == setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))
            {
                perror("Error setting socket options");
                to_close();
                return -1;
            }
            return event_loop();
        }

        int udp_component::udp_component_client(const std::string &IP,
                                                const int PORT,
                                                const char *buffer,
                                                ssize_t len,
                                                struct sockaddr *addr /*=nullptr*/,
                                                socklen_t addr_len /*=0*/)
        {
            int &client_socket = m_socket_fd;
            if (client_socket <= 0)
            {
                if (0 != init_sock())
                {
                    perror("Error creating socket");
                    return -1;
                }
            }

            sockaddr_in server_addr;
            std::memset(&server_addr, 0, sizeof(server_addr));

            // server addr
            if (IP != "" && PORT != 0)
            {
                server_addr.sin_family = AF_INET;
                server_addr.sin_addr.s_addr = inet_addr(IP.c_str());
                server_addr.sin_port = htons(PORT);
            }
            else if (addr && addr_len > 0)
            {
            }
            else
            {
                perror("IP PORT and addr addr_len");
                return -1;
            }

            ssize_t bytesSent = 0;

            if (addr)
            {
                bytesSent = sendto(client_socket, buffer, len, 0,
                                   addr, addr_len);
            }
            else
            {
                bytesSent = sendto(client_socket, buffer, len, 0,
                                   (sockaddr *)&server_addr, sizeof(server_addr));
            }

            if (bytesSent != len)
            {
                perror("Error sending message bytesSent != len");
                to_close();
                return -1;
            }

            if (addr == nullptr)
            {
                return event_loop();
            }

            return 0;
        }

        int udp_component::event_loop()
        {
            int &server_socket_fd = m_socket_fd;
            if (server_socket_fd <= 0)
            {
                perror("eventloop err server_socket_fd <= 0");
                return -1;
            }

            // set nonblocking
            if (udp_component_setnonblocking(server_socket_fd) == -1)
            {
                to_close();
                perror("udp_component_setnonblocking err");
                return -1;
            }

            // create epoll instance
            m_epoll_fd = epoll_create(1);
            int &epoll_fd = m_epoll_fd;
            if (epoll_fd == -1)
            {
                perror("error creating epoll");
                to_close();
                return -1;
            }

            // add server fd to epoll's listen list
            epoll_event event;
            bzero(&event, sizeof(event));
            event.data.fd = server_socket_fd;
            event.events = EPOLLIN;
            if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_fd, &event))
            {
                perror("error adding server socket to epoll");
                to_close();
                return -1;
            }

            // event loop
            constexpr int MAX_EVENTS_NUM = 1;
            sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            epoll_event events[MAX_EVENTS_NUM];
            bzero(&events[0], sizeof(events));
            int events_num = 0;

            bzero(&client_addr, sizeof(client_addr));
            constexpr int buffer_size = 65507;
            char buffer[buffer_size];
            bzero(buffer, sizeof(buffer));

            do
            {
                events_num = epoll_wait(epoll_fd, events, MAX_EVENTS_NUM, 10); // 10ms

                // tick
                {
                    if (tick_callback)
                    {
                        bool to_stop = false;
                        tick_callback(to_stop);
                        if (to_stop)
                        {
                            to_close();
                            return 0;
                        }
                    }
                }

                if (0 == events_num)
                {
                    continue;
                }
                else if (0 > events_num)
                {
                    perror("0 > events_num");
                    to_close();
                    return -1;
                }

                for (int i = 0; i < events_num; ++i)
                {
                    if (events[i].data.fd == server_socket_fd)
                    {
                        // have new message
                        ssize_t bytes = recvfrom(server_socket_fd, buffer, buffer_size, 0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);

                        // process recved data
                        if (bytes > 0)
                        {
                            if (message_callback)
                            {
                                message_callback(buffer, bytes, client_addr);
                            }
                        }
                    }
                }
            } while (true);
            return 0;
        }
    }
}
