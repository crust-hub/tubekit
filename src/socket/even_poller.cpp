#include "socket/event_poller.h"

using namespace tubekit::socket;

event_poller::event_poller(bool et) : m_epfd(0), m_et(et), m_max_connections(0), m_events(nullptr)
{
}

event_poller::~event_poller()
{
    if (m_epfd > 0)
    {
        if (m_epfd != 0)
            close(m_epfd);
        m_epfd = 0;
    }
    if (m_events != nullptr)
    {
        delete[] m_events;
        m_events = nullptr;
    }
}

void event_poller::create(int max_connections)
{
    m_max_connections = max_connections;
    m_epfd = ::epoll_create(max_connections + 1); // plane
    if (m_epfd < 0)
    {
        return;
    }
    if (m_events != nullptr)
    {
        delete[] m_events;
        m_events = nullptr;
    }
    m_events = new ::epoll_event[max_connections + 1];
}

/**
 * @brief 操作epoll句柄
 *
 * @param fd epoll句柄
 * @param ptr 创建的epoll_event携带的数据指针
 * @param events EPOLLIN表示对应的文件描述符可以读,
 *               EPOLLOUT表示对应的文件描述符可以写，
 *               EPOLLPRI表示对应的文件描述符有紧急的数可读，
 *               EPOLLERR：表示对应的文件描述符发生错误，
 *               EPOLLHUP表示对应的文件描述符被挂断，
 *               EPOLLET ET的epoll工作模式
 *               EPOLLLT表示默认epoll工作模式
 * @param op 操作选项 EPOLL_CTL_ADD，EPOLL_CTL_MOD，EPOLL_CTL_DEL
 */
void event_poller::ctrl(int fd, void *ptr, __uint32_t events, int op)
{
    struct ::epoll_event ev;
    ev.data.ptr = ptr;
    if (m_et)
    {
        ev.events = events | EPOLLET;
        /*
        epoll是linux系统最新的处理多连接的高效率模型， 工作在两种方式下， EPOLLLT方式和EPOLLET方式。
        EPOLLLT是系统默认， 工作在这种方式下， 程序员不易出问题， 在接收数据时，只要socket输入缓存有数据，
        都能够获得EPOLLIN的持续通知， 同样在发送数据时， 只要发送缓存够用， 都会有持续不间断的EPOLLOUT
        通知。而对于EPOLLET是另外一种触发方式， 比EPOLLLT要高效很多， 对程序员的要求也多些， 程序员必须
        小心使用，因为工作在此种方式下时， 在接收数据时， 如果有数据只会通知一次， 假如read时未读完数据，
        那么不会再有EPOLLIN的通知了， 直到下次有新的数据到达时为止； 当发送数据时， 如果发送缓存未满也只有
        一次EPOLLOUT的通知， 除非你把发送缓存塞满了， 才会有第二次EPOLLOUT通知的机会， 所以在此方式下
        read和write时都要处理好。 暂时写到这里， 留作备忘。
        如果将一个socket描述符添加到两个epoll中， 那么即使在EPOLLET模式下， 只要前一个epoll_wait时，
        未读完， 那么后一个epoll_wait事件时， 也会得到读的通知， 但前一个读完的情况下， 后一个epoll就不会得到读事件的通知了
        */
    }
    else
    {
        ev.events = events;
    }
    epoll_ctl(m_epfd, op, fd, &ev);
}

void event_poller::add(int fd, void *ptr, __uint32_t events)
{
    ctrl(fd, ptr, events, EPOLL_CTL_ADD);
}

void event_poller::mod(int fd, void *ptr, __uint32_t events)
{
    ctrl(fd, ptr, events, EPOLL_CTL_MOD);
}

void event_poller::del(int fd, void *ptr, __uint32_t events)
{
    ctrl(fd, ptr, events, EPOLL_CTL_DEL);
}

int event_poller::wait(int millsecond)
{
    return epoll_wait(m_epfd, m_events, m_max_connections + 1, millsecond);
}
