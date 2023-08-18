#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <tubekit-log/logger.h>

#include "socket/socket.h"
#include "utility/singleton.h"

using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::log;

socket::socket() : m_sockfd(0)
{
}

socket::socket(const string &ip, int port) : m_ip(ip), m_port(port), m_sockfd(0)
{
}

socket::~socket()
{
    close();
}

bool socket::bind(const string &ip, int port)
{
    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr)); // init 0
    sockaddr.sin_family = AF_INET;          // IPV4
    if (ip != "")
    {
        sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    }
    else
    {
        sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    }
    // htonl htons : change to net byte sequeue from host byte
    sockaddr.sin_port = htons(port);
    if (::bind(m_sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
    {
        LOG_ERROR("socket bind error: errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::listen(int backlog)
{
    // backlog: queue of pending connections
    if (::listen(m_sockfd, backlog) < 0)
    {
        LOG_ERROR("socket listen error: errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::connect(const string &ip, int port)
{
    struct sockaddr_in sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);
    if (::connect(m_sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
    {
        LOG_ERROR("socket connect error: errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::close()
{
    if (close_callback)
    {
        close_callback();
    }
    if (m_sockfd > 0)
    {
        ::close(m_sockfd);
        m_sockfd = 0;
    }
    return true;
}

int socket::accept()
{
    int sockfd = ::accept(m_sockfd, NULL, NULL);
    if (sockfd < 0)
    {
        LOG_ERROR("accept call error: errno=%d errstr=%s", errno, strerror(errno));
        sockfd = -1;
    }
    return sockfd;
}

int socket::recv(char *buf, size_t len)
{
    // read len bytes from m_sockfd
    return ::recv(m_sockfd, buf, len, 0);
}

int socket::send(const char *buf, size_t len)
{
    // write data to m_sockfd
    return ::send(m_sockfd, buf, len, 0);
}

bool socket::set_non_blocking()
{
    int flags = fcntl(m_sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        LOG_ERROR("socket::set_non_blocking(F_GETFL,O_NONBLOCK) errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    flags |= O_NONBLOCK; // setting nonblock
    if (fcntl(m_sockfd, F_SETFL, flags) < 0)
    {
        LOG_ERROR("socket::set_non_blocking(F_SETFL,O_NONBLOCK) errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::set_blocking()
{
    int flags = fcntl(m_sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        LOG_ERROR("socket::set_blocking() errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    flags &= ~O_NONBLOCK; // setting nonblock
    if (fcntl(m_sockfd, F_SETFL, flags) < 0)
    {
        LOG_ERROR("socket::set_blocking() errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::set_send_buffer(size_t size)
{
    size_t buffer_size = size;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size)) < 0)
    {
        LOG_ERROR("socket set send buffer error: errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::set_recv_buffer(size_t size)
{
    int buffer_size = size;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0)
    {
        LOG_ERROR("socket set recv buffer errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::set_linger(bool active, size_t seconds)
{
    // l_onoff = 0; l_linger ignore
    // close()

    // l_onoff != 0;
    // l_linger = 0;
    // close()

    // l_onoff != 0;
    // l_linger > 0;
    // close()
    struct linger l;
    memset(&l, 0, sizeof(l));
    if (active)
        l.l_onoff = 1;
    else
        l.l_onoff = 0;
    l.l_linger = seconds;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) < 0)
    {
        LOG_ERROR("socket set linger error errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::set_keep_alive()
{
    int flag = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag)) < 0)
    {
        LOG_ERROR("socket set sock keep alive error: errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::set_reuse_addr()
{
    int flag = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
    {
        LOG_ERROR("socket set sock reuser addr error: errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket::set_reuse_port()
{
    int flag = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0)
    {
        LOG_ERROR("socket set sock reuser port error: errno=%d errstr=%s", errno, strerror(errno));
        return false;
    }
    return true;
}

int socket::create_tcp_socket()
{
    int fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        LOG_ERROR("create tcp socket error: errno=%d errstr=%s", errno, strerror(errno));
        return fd;
    }
    return fd;
}

int socket::get_fd()
{
    return this->m_sockfd;
}