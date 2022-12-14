#include "socket_handler.h"
#include "server_socket.h"
#include "thread/auto_lock.h"
#include "utility/singleton_template.h"
#include "thread/task_dispatcher.h"
#include "task/task_factory.h"
#include "log/logger.h"
#include "server/server.h"

using namespace std;
using namespace tubekit::socket;
using namespace tubekit::thread;
using namespace tubekit::task;
using namespace tubekit::log;
using namespace tubekit::utility;
using namespace tubekit::server;

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
    m_epoll = new event_poller(false); // EPOLLLT mode
    m_epoll->create(max_connections);
    m_epoll->add(m_server->m_sockfd, m_server, (EPOLLIN | EPOLLHUP | EPOLLERR)); // Register the native socket epoll_event
    socket_pool.init(max_connections);
    // main thread loop
    while (true)
    {
        int num = m_epoll->wait(wait_time);
        if (num == 0)
        {
            continue;
        }
        for (int i = 0; i < num; i++) // Sockets that handle readable data
        {
            // There is a new socket connection
            if (m_server == static_cast<socket *>(m_epoll->m_events[i].data.ptr))
            {
                int socket_fd = m_server->accept(); // Gets the socket_fd for the new connection
                socket *socket_object = socket_pool.allocate();
                if (socket_object == nullptr)
                {
                    break;
                }
                socket_object->m_sockfd = socket_fd;
                socket_object->set_non_blocking();
                attach(socket_object);
            }
            else // Data sent by the client can be read
            {
                socket *socketfd = static_cast<socket *>(m_epoll->m_events[i].data.ptr);
                // Different processing is triggered for different poll events
                if (m_epoll->m_events[i].events & EPOLLHUP) // The file descriptor is hung up
                {
                    detach(socketfd);
                }
                else if (m_epoll->m_events[i].events & EPOLLERR) // An error occurred with the file descriptor
                {
                    detach(socketfd);
                    remove(socketfd);
                }
                else if (m_epoll->m_events[i].events & EPOLLIN) // There is data,to be can read
                {
                    detach(socketfd);
                    // Decide which engine to use,such as WORKDLOW_TASK or HTTP_TASK
                    std::string task_type = singleton_template<server::server>::instance()->get_task_type();
                    thread::task *new_task = nullptr;
                    if (task_type == "WORKFLOW_TASK")
                        new_task = task_factory::create(socketfd, task_factory::WORKFLOW_TASK);
                    else if (task_type == "HTTP_TASK")
                        new_task = task_factory::create(socketfd, task_factory::HTTP_TASK);
                    singleton_template<logger>::instance()->debug(__FILE__, __LINE__, "new work task submit to task_dispatcher");
                    if (new_task == nullptr)
                    {
                        singleton_template<logger>::instance()->error(__FILE__, __LINE__, "new_task is nullptr");
                    }
                    else
                    {
                        // Submit the task to the queue of task_dispatcher
                        singleton_template<task_dispatcher<work_thread, thread::task>>::instance()->assign(new_task);
                    }
                }
            }
        }
    }
}