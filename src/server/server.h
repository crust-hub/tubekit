#pragma once
#include <string>

namespace tubekit
{
    namespace server
    {
        class server
        {
        public:
            server();
            ~server() = default;

            void listen(const std::string &ip, int port);
            void start();
            void set_threads(size_t threads);
            void set_connects(size_t connects);
            void set_wait_time(size_t wait_time);
            void config(const std::string &ip, int port, size_t threads, size_t connects, size_t wait_time);

        private:
            std::string m_ip;
            size_t m_port;
            size_t m_threads;
            size_t m_connects;
            size_t m_wait_time;
        };
    }
}