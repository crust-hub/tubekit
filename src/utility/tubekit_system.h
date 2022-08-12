#pragma once
#include <string>

namespace tubekit
{
    namespace utility
    {
        using namespace std;
        class tubekit_system
        {
        public:
            tubekit_system() = default;
            ~tubekit_system() = default;
            void init();            // init tubekit system
            string get_root_path(); // get executable file location path

        private:
            void core_dump(); // core dump

        private:
            string m_root_path;
        };
    }
}