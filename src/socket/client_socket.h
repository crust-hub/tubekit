#pragma once
#include <string>

#include "socket/socket.h"

namespace tubekit
{
    namespace socket
    {
        class client_socket : public socket
        {
        public:
            client_socket();
            client_socket(const string &ip, int port);
            virtual ~client_socket();
        };
    }
}