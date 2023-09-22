#include <tubekit-log/logger.h>

#include "socket/socket_handler.h"
#include "socket/server_socket.h"
#include "thread/auto_lock.h"
#include "utility/singleton.h"
#include "thread/worker_pool.h"
#include "task/task_factory.h"
#include "server/server.h"
#include "app/tick.h"
#include "app/stop.h"
#include "system/system.h"
#include "connection/connection.h"
#include "connection/connection_mgr.h"
#include "connection/http_connection.h"
#include "connection/stream_connection.h"

using namespace std;
using namespace tubekit::socket;
using namespace tubekit::thread;
using namespace tubekit::task;
using namespace tubekit::log;
using namespace tubekit::utility;
using namespace tubekit::server;
using namespace tubekit::app;
using namespace tubekit::connection;

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

int socket_handler::attach(socket *m_socket, bool listen_send /*= false*/)
{
    auto_lock lock(m_mutex);
    uint_least32_t events = 0;
    if (listen_send)
    {
        events = (EPOLLONESHOT | EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR);
    }
    else
    {
        events = (EPOLLONESHOT | EPOLLIN | EPOLLHUP | EPOLLERR);
    }
    int i_ret = m_epoll->add(m_socket->m_sockfd, (void *)m_socket, events);
    if (0 == i_ret)
    {
        return 0;
    }
    // using EPOLL_CTL_MOD
    if (!(i_ret == -1 && errno == EEXIST))
    {
        return i_ret;
    }
    return m_epoll->mod(m_socket->m_sockfd, (void *)m_socket, events);
}

int socket_handler::detach(socket *m_socket)
{
    auto_lock lock(m_mutex);
    return m_epoll->del(m_socket->m_sockfd, (void *)m_socket, 0);
}

int socket_handler::remove(socket *m_socket)
{
    int iret = detach(m_socket);
    if (0 != iret)
    {
        // LOG_ERROR("detach(m_socket) return %d", iret);
    }
    m_socket->close();
    socket_pool.release(m_socket); // return back to socket object poll
    return iret;
}

socket *socket_handler::alloc_socket()
{
    return socket_pool.allocate();
}

void socket_handler::on_tick()
{
    singleton<app::tick>::instance()->run();
}

void socket_handler::handle(int max_connections, int wait_time)
{
    m_epoll = new event_poller(false); // EPOLLLT mode
    m_epoll->create(max_connections);
    m_epoll->add(m_server->m_sockfd, m_server, (EPOLLIN | EPOLLHUP | EPOLLERR)); // Register the listen socket epoll_event
    socket_pool.init(max_connections);
    // main thread loop
    while (true)
    {
        // sys stop check
        if (singleton<tubekit::server::server>::instance()->is_stop())
        {
            singleton<tubekit::server::server>::instance()->on_stop();
            singleton<app::stop>::instance()->run();
            break; // main process to exit
        }
        int num = m_epoll->wait(wait_time);
        on_tick();
        if (num == 0)
        {
            continue; // timeout
        }
        else if (num < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }

        for (int i = 0; i < num; i++) // Sockets that handle readable data
        {
            // There is a new socket connection
            if (m_server == static_cast<socket *>(m_epoll->m_events[i].data.ptr))
            {
                int socket_fd = m_server->accept(); // Gets the socket_fd for the new connection
                socket *socket_object = alloc_socket();
                if (socket_object == nullptr)
                {
                    continue;
                }
                socket_object->m_sockfd = socket_fd;
                socket_object->close_callback = nullptr;
                socket_object->set_non_blocking();
                socket_object->set_linger(false, 0);

                // create connection layer instance
                auto task_type = singleton<server::server>::instance()->get_task_type();
                connection::connection *p_connection = nullptr;
                {
                    switch (task_type)
                    {
                    case server::server::STREAM_TASK:
                        p_connection = new connection::stream_connection(socket_object);
                        if (p_connection == nullptr)
                        {
                            LOG_ERROR("new connection::stream_connection error");
                        }
                        break;
                    case server::server::HTTP_TASK:
                        p_connection = new connection::http_connection(socket_object);
                        if (p_connection == nullptr)
                        {
                            LOG_ERROR("new connection::http_connection error");
                        }
                        break;
                    default:
                        break;
                    }
                }

                if (p_connection == nullptr)
                {
                    remove(socket_object);
                    continue;
                }

                bool bret = singleton<connection_mgr>::instance()->add(socket_object, p_connection);
                if (!bret)
                {
                    LOG_ERROR("singleton<connection_mgr>::instance()->add error");
                    delete p_connection;
                    remove(socket_object);
                    continue;
                }

                attach(socket_object); // listen read
            }
            else
            {
                // already connection socket process
                socket *socket_ptr = static_cast<socket *>(m_epoll->m_events[i].data.ptr);
                detach(socket_ptr);
                // get connection layer instance
                connection::connection *p_connection = singleton<connection_mgr>::instance()->get(socket_ptr);
                if (p_connection == nullptr)
                {
                    remove(socket_ptr);
                    continue;
                }

                if (m_epoll->m_events[i].events & EPOLLHUP) // The file descriptor is hung up,client closed
                {
                    p_connection->mark_close();
                }

                if (m_epoll->m_events[i].events & EPOLLERR) // An error occurred with the file descriptor
                {
                    p_connection->mark_close();
                }

                // Different processing is triggered for different poll events
                bool recv_event = false;
                bool send_event = false;
                if ((m_epoll->m_events[i].events & EPOLLIN) || (m_epoll->m_events[i].events & EPOLLOUT)) // There is data,to be can read
                {
                    recv_event = (m_epoll->m_events[i].events & EPOLLIN);
                    send_event = (m_epoll->m_events[i].events & EPOLLOUT);
                }

                // Decide which engine to use,such as WORKDLOW_TASK or HTTP_TASK
                auto task_type = singleton<server::server>::instance()->get_task_type();
                thread::task *new_task = nullptr;

                // create task
                switch (task_type)
                {
                case server::server::STREAM_TASK:
                    new_task = task_factory::create(socket_ptr, task_factory::STREAM_TASK);
                    if (new_task)
                    {
                        auto stream_task_ptr = (stream_task *)new_task;
                        stream_task_ptr->reason_recv = recv_event;
                        stream_task_ptr->reason_send = send_event;
                    }
                    break;
                case server::server::HTTP_TASK:
                    new_task = task_factory::create(socket_ptr, task_factory::HTTP_TASK);
                    if (new_task)
                    {
                        auto http_task_ptr = (http_task *)new_task;
                        http_task_ptr->reason_recv = recv_event;
                        http_task_ptr->reason_send = send_event;
                    }
                    break;
                default:
                    break;
                }

                // create task failed
                if (new_task == nullptr)
                {
                    remove(socket_ptr);
                    LOG_ERROR("new_task is nullptr");
                    singleton<connection_mgr>::instance()->remove(socket_ptr);
                    continue;
                }

                // Submit the task to the queue of task_dispatcher
                singleton<worker_pool>::instance()->assign(new_task);
            }
        }
    }
}
