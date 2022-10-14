#pragma once
#include <string>

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

        private:
            void core_dump(); // core dump

        private:
            std::string m_root_path;
        };
    }
}