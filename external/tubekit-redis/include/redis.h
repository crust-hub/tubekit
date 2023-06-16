#pragma once
#include <string>

namespace tubekit
{
    namespace redis
    {
        class redis
        {
        public:
            redis(const std::string &host, const std::string &pwd, const int &port = 6379);
            ~redis();
            void test();

        private:
            std::string host;
            int port;
            std::string pwd;
        };
    }
}