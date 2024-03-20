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
        class udp_component
        {
        public:
            ~udp_component();
            std::string udp_component_get_ip(struct sockaddr_in &addr);
            int udp_component_get_port(struct sockaddr_in &addr);

            int udp_component_server(const std::string &IP,
                                     const int PORT);

            int udp_component_client(const std::string &IP,
                                     const int PORT,
                                     const char *buffer,
                                     ssize_t len,
                                     struct sockaddr *addr = nullptr,
                                     socklen_t addr_len = 0);

        private:
            int event_loop();
            int init_sock();
            void to_close();

        private:
            int m_socket_fd{0};
            int m_epoll_fd{0};

        public:
            std::function<void(bool &to_stop)> tick_callback{nullptr};
            std::function<void(const char *buffer, ssize_t len, struct sockaddr_in &)> message_callback{nullptr};
            std::function<void()> close_callback{nullptr};
        };
    }
}
