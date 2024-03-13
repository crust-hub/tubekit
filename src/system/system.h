#pragma once
#include <string>
#include <signal.h>

namespace tubekit
{
    namespace system
    {
        class system
        {
        public:
            system() = default;
            ~system() = default;
            void init();                 // init tubekit system
            std::string get_root_path(); // get executable file location path
            static void signal_term(int sig);

        private:
            int set_sys_limits();
            void signal_conf();
            void create_daemon();

        private:
            std::string m_root_path{};
        };
    }
}