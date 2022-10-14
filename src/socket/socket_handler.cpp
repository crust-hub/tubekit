#include "socket_handler.h"
#include "server_socket.h"
#include "thread/auto_lock.h"
#include "utility/singleton_template.h"
#include "thread/task_dispatcher.h"
#include "task/task_factory.h"
#include "log/logger.h"

using namespace std;
using namespace tubekit::socket;
using namespace tubekit::thread;
using namespace tubekit::task;
using namespace tubekit::log;
using namespace tubekit::utility;

socket_handler::socket_handler()
{
}

socket_handler::~socket_handler()
{
    if (m_epoll != nullptr)
    {
        delete m_epoll;
        m_epoll = nullptr;
    }
    if (m_server != nullptr)
    {
        delete m_server;
        m_epoll = nullptr;
    }
}

void socket_handler::listen(const string &ip, int port)
{
    m_server = new server_socket(ip, port);
}

void socket_handler::attach(socket *m_socket)
{
    auto_lock lock(m_mutex);
    m_epoll->add(m_socket->m_sockfd, (void *)m_socket, (EPOLLONESHOT | EPOLLIN | EPOLLHUP | EPOLLERR));
}

void socket_handler::detach(socket *m_socket)
{
    auto_lock lock(m_mutex);
    m_epoll->del(m_socket->m_sockfd, (void *)m_socket, (EPOLLONESHOT | EPOLLIN | EPOLLHUP | EPOLLERR));
}

void socket_handler::remove(socket *m_socket)
{
    m_socket->close();
    socket_pool.release(m_socket);
}

void socket_handler::handle(int max_connections, int wait_time)
{
    m_epoll = new event_poller(false); // EPOLLLT模式
    m_epoll->create(max_connections);
    m_epoll->add(m_server->m_sockfd, m_server, (EPOLLIN | EPOLLHUP | EPOLLERR)); //将本机socket注册epoll_event
    socket_pool.init(max_connections);
    //主线程循环
    while (true)
    {
        int num = m_epoll->wait(wait_time);
        if (num == 0)
        {
            continue;
        }
        for (int i = 0; i < num; i++) //处理可读出数据的socket
        {
            //有新的socket连接
            if (m_server == static_cast<socket *>(m_epoll->m_events[i].data.ptr))
            {
                int socket_fd = m_server->accept(); //获取新连接的socket_fd
                socket *socket_object = socket_pool.allocate();
                if (socket_object == nullptr)
                {
                    break;
                }
                socket_object->m_sockfd = socket_fd;
                socket_object->set_non_blocking();
                attach(socket_object);
            }
            else //可读取客户端发来的数据
            {
                socket *socketfd = static_cast<socket *>(m_epoll->m_events[i].data.ptr);
                //针对不同的事件触发不同的处理
                if (m_epoll->m_events[i].events & EPOLLHUP) //文件描述符被挂断
                {
                    detach(socketfd);
                }
                else if (m_epoll->m_events[i].events & EPOLLERR) //文件描述符发生错误
                {
                    detach(socketfd);
                    remove(socketfd);
                }
                else if (m_epoll->m_events[i].events & EPOLLIN) //有数据可读
                {
                    detach(socketfd);
                    thread::task *new_task = task_factory::create(socketfd, task_factory::WORK_TASK);
                    singleton_template<logger>::instance()->debug(__FILE__, __LINE__, "new work task submit to task_dispatcher");
                    //提交给work_thread所处理的任务队列
                    singleton_template<task_dispatcher<work_thread, thread::task>>::instance()->assign(new_task);
                }
            }
        }
    }
}