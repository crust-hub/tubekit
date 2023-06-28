#include <cerrno>
#include <cstring>
#include <tubekit-log/logger.h>

#include "socket/server_socket.h"
#include "socket/socket.h"
#include "utility/singleton_template.h"

using namespace std;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::log;

server_socket::server_socket() : socket()
{
}

server_socket::server_socket(const string &ip, int port) : socket(ip, port)
{
    m_sockfd = socket::create_tcp_socket();
    if (m_sockfd < 0)
    {
        singleton_template<logger>::instance()->error("create server socket error: errno=%d errstr=%s", errno, strerror(errno));
        return;
    }
    set_non_blocking();
    set_recv_buffer(10 * 1024); // 10MB
    set_send_buffer(10 * 1024); // 10MB
    set_linger(true, 0);
    set_keep_alive();
    set_reuse_addr();
    set_reuse_port();
    bind(ip, port);
    listen(1024);
}

server_socket::~server_socket()
{
    close();
}
