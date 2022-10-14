#include <cerrno>
#include <cstring>

#include "socket/client_socket.h"
#include "log/logger.h"
#include "utility/singleton_template.h"

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
        singleton_template<logger>::instance()->error("create client socket error: errno=%d errstr=%s", errno, strerror(errno));
        return;
    }
    connect(ip, port);
}

client_socket::~client_socket()
{
    close();
}
