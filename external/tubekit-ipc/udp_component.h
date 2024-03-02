#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <string>
#include <sys/epoll.h>
#include <functional>

namespace tubekit
{
    namespace ipc
    {
        int udp_component_server(const std::string &IP,
                                 const std::string &PORT,
                                 const int event_loop_ms,
                                 bool &stop_flag,
                                 std::function<void()> tick_callback,
                                 std::function<void(const char *buffer, ssize_t len, struct sockaddr_in &)> message_callback,
                                 std::function<void()> close_callback);

        int udp_component_client(const std::string &IP,
                                 const int PORT,
                                 const char *buffer,
                                 ssize_t len,
                                 const std::string &CLIENT_IP = "",
                                 const int CLIENT_PORT = 0);

        std::string udp_component_get_ip(struct sockaddr_in &addr);

        int udp_component_get_port(struct sockaddr_in &addr);
    }
}

