#pragma once
#include <string>

namespace tubekit
{
    namespace socket
    {
        using namespace std;

        class socket
        {
        public:
            virtual ~socket();
            socket();
            socket(const string &ip, int port);
            bool bind(const string &ip, int port);

            /**
             * @brief socket listen
             *
             * @param backlog 挂起的连接队列的最大长度
             * @return true
             * @return false
             */
            bool listen(int backlog);

            bool connect(const string &ip, int port);
            bool close();
            int accept();
            int recv(char *buf, size_t len);
            int send(const char *buf, size_t len);
            bool set_non_blocking();
            bool set_send_buffer(size_t size);
            bool set_recv_buffer(size_t size);
            bool set_linger(bool active, size_t seconds);
            /**
             * @brief heart beat config
             *
             * @return true
             * @return false
             */
            bool set_keep_alive();
            bool set_reuse_addr();
            bool set_reuse_port();

        protected:
            string m_ip;
            int m_port;
            int m_sockfd;

        public:
            static int create_tcp_socket();
        };
    }
}