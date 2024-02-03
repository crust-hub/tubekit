#pragma once
#include <string>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>

#include "task/task_type.h"

namespace tubekit
{
    namespace server
    {
        class server
        {
        public:
            server();
            ~server();

            void listen(const std::string &ip, int port);
            void start();
            void set_threads(size_t threads);
            void set_connects(size_t connects);
            void set_wait_time(size_t wait_time);
            void set_task_type(std::string task_type);

            void set_http_static_dir(std::string http_static_dir);
            const std::string &get_http_static_dir();

            void set_daemon(bool daemon);
            void set_use_ssl(bool use_ssl);
            void set_crt_pem(std::string set_crt_pem);
            void set_key_pem(std::string set_key_pem);

            bool get_use_ssl();
            std::string get_crt_pem();
            std::string get_key_pem();

            SSL_CTX *get_ssl_ctx();

            void config(const std::string &ip,
                        int port,
                        size_t threads,
                        size_t connects,
                        size_t wait_time,
                        std::string task_type,
                        std::string http_static_dir,
                        bool daemon = false,
                        std::string crt_pem = "",
                        std::string key_pem = "",
                        bool use_ssl = false);
            enum tubekit::task::task_type get_task_type();
            bool on_stop();
            void to_stop();
            bool is_stop();

        private:
            std::string m_ip;
            size_t m_port;
            size_t m_threads;
            size_t m_connects;
            size_t m_wait_time;
            std::string m_http_static_dir;

            std::string m_task_type;
            bool m_daemon{false};
            bool m_use_ssl{false};
            std::string m_crt_pem;
            std::string m_key_pem;
            volatile bool stop_flag{false};
            SSL_CTX *m_ssl_context{nullptr};
        };
    }
}