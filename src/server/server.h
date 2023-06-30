#pragma once
#include <string>

namespace tubekit
{
    namespace server
    {
        class server
        {
        public:
            enum TaskType
            {
                HTTP_TASK = 0,
                STREAM_TASK,
                NONE
            };

        public:
            server();
            ~server() = default;

            void listen(const std::string &ip, int port);
            void start();
            void set_threads(size_t threads);
            void set_connects(size_t connects);
            void set_wait_time(size_t wait_time);
            void set_task_type(std::string task_type);
            void set_daemon(bool daemon);
            void config(const std::string &ip,
                        int port,
                        size_t threads,
                        size_t connects,
                        size_t wait_time,
                        std::string task_type,
                        bool daemon = false);
            enum TaskType get_task_type();

        private:
            std::string m_ip;
            size_t m_port;
            size_t m_threads;
            size_t m_connects;
            size_t m_wait_time;
            std::string m_task_type;
            bool m_daemon{false};
        };
    }
}