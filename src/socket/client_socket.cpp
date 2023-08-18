#include <cerrno>
#include <cstring>
#include <tubekit-log/logger.h>

#include "socket/client_socket.h"
#include "utility/singleton.h"

using namespace std;
using namespace tubekit::utility;
using namespace tubekit::socket;
using namespace tubekit::log;

client_socket::client_socket() : socket()
{
}

client_socket::client_socket(const string &ip, int port) : socket(ip, port)
{
    m_sockfd = socket::create_tcp_socket();
    if (m_sockfd < 0)
    {
        LOG_ERROR("create client socket error: errno=%d errstr=%s", errno, strerror(errno));
        return;
    }
    connect(ip, port);
}

client_socket::~client_socket()
{
    close();
}
