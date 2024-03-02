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
                std::cerr << "error getting fd flags" << std::endl;
                return -1;
            }

            if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            {
                std::cerr << "error setting non-bloking mode" << std::endl;
                return -1;
            }

            return 0;
        }

        int udp_component_server(const std::string &IP,
                                 const std::string &PORT,
                                 const int event_loop_ms,
                                 bool &stop_flag,
                                 std::function<void()> tick_callback,
                                 std::function<void(const char *buffer, ssize_t len, struct sockaddr_in &)> message_callback,
                                 std::function<void()> close_callback)
        {
            const int int_port = std::atoi(PORT.c_str());

            // create udp socket
            int server_socket_fd = 0;
            server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (server_socket_fd == -1)
            {
                std::cerr << "error creating socket" << std::endl;
                return EXIT_FAILURE;
            }

            // setting server addr
            struct sockaddr_in server_addr, client_addr;
            bzero(&server_addr, sizeof(server_addr));
            bzero(&client_addr, sizeof(client_addr));

            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr(IP.c_str());
            server_addr.sin_port = htons(int_port);

            // server bind addr
            if (-1 == bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
            {
                std::cerr << "error binding socket" << std::endl;
                close(server_socket_fd);
                return EXIT_FAILURE;
            }

            // port reuse
            int reuse = 1;
            if (-1 == setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))
            {
                perror("Error setting socket options");
                close(server_socket_fd);
                return EXIT_FAILURE;
            }

            // set nonblocking
            if (udp_component_setnonblocking(server_socket_fd) == -1)
            {
                close(server_socket_fd);
                return EXIT_FAILURE;
            }

            // create epoll instance
            int epoll_fd = epoll_create(1);
            if (epoll_fd == -1)
            {
                std::cerr << "error creating epoll" << std::endl;
                close(server_socket_fd);
                return EXIT_FAILURE;
            }

            // add server fd to epoll's listen list
            epoll_event event;
            bzero(&event, sizeof(event));
            event.data.fd = server_socket_fd;
            event.events = EPOLLIN;
            if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_fd, &event))
            {
                std::cerr << "error adding server socket to epoll" << std::endl;
                close(epoll_fd);
                close(server_socket_fd);
                return EXIT_FAILURE;
            }

            // event loop
            constexpr int MAX_EVENTS_NUM = 1;
            socklen_t addr_len = sizeof(client_addr);
            epoll_event events[MAX_EVENTS_NUM];
            bzero(&events[0], sizeof(events));
            int events_num = 0;

            bzero(&client_addr, sizeof(client_addr));
            char buffer[65507];
            bzero(buffer, sizeof(buffer));

            do
            {
                if (stop_flag)
                {
                    break;
                }

                events_num = epoll_wait(epoll_fd, events, MAX_EVENTS_NUM, event_loop_ms);

                // tick
                {
                    if (tick_callback)
                    {
                        tick_callback();
                    }
                }

                if (0 >= events_num)
                {
                    continue;
                }
                for (int i = 0; i < events_num; ++i)
                {
                    if (events[i].data.fd == server_socket_fd)
                    {
                        // have new message
                        ssize_t bytes = recvfrom(server_socket_fd, buffer, 65507, 0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);

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

            close(epoll_fd);
            close(server_socket_fd);
            if (close_callback)
            {
                close_callback();
            }
        }

        int udp_component_client(const std::string &IP,
                                 const int PORT,
                                 const char *buffer,
                                 ssize_t len,
                                 const std::string &CLIENT_IP /*= ""*/,
                                 const int CLIENT_PORT /*= 0*/)
        {
            int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
            if (client_socket == -1)
            {
                perror("Error creating socket");
                return EXIT_FAILURE;
            }

            sockaddr_in client_addr, server_addr;
            std::memset(&client_addr, 0, sizeof(client_addr));
            std::memset(&server_addr, 0, sizeof(server_addr));

            if (CLIENT_IP != "" && CLIENT_PORT != 0)
            {
                client_addr.sin_family = AF_INET;
                client_addr.sin_addr.s_addr = inet_addr(CLIENT_IP.c_str());
                client_addr.sin_port = htons(CLIENT_PORT);
                // bind client
                if (-1 == bind(client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr)))
                {
                    perror("Error binding local socket");
                    close(client_socket);
                    return EXIT_FAILURE;
                }

                // port reuse
                int reuse = 1;
                if (-1 == setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))
                {
                    perror("Error setting socket options");
                    close(client_socket);
                    return EXIT_FAILURE;
                }
            }

            // server addr
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr(IP.c_str());
            server_addr.sin_port = htons(PORT);

            ssize_t bytesSent = sendto(client_socket, buffer, len, 0,
                                       (struct sockaddr *)&server_addr, sizeof(server_addr));

            if (-1 == bytesSent)
            {
                perror("Error sending message");
                close(client_socket);
                return EXIT_FAILURE;
            }

            close(client_socket);

            return 0;
        }

        std::string udp_component_get_ip(struct sockaddr_in &addr)
        {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
            return std::string(ip, INET_ADDRSTRLEN);
        }

        int udp_component_get_port(struct sockaddr_in &addr)
        {
            unsigned short port = ntohs(addr.sin_port);
            return port;
        }
    }
}
