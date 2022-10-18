#include <iostream>

#include "server.h"
#include "thread/task_dispatcher.h"
#include "thread/work_thread.h"
#include "thread/task.h"
#include "socket/socket_handler.h"

using namespace std;
using namespace tubekit::server;
using namespace tubekit::socket;

server::server() : m_ip("0.0.0.0"),
                   m_port(0),
                   m_threads(1024),
                   m_connects(1024),
                   m_wait_time(10)
{
}

void server::listen(const std::string &ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void server::start()
{
    cout << "server start..." << endl;
    // init thread pool and task queue,task_dispather is detached thread to processing task in queue
    task_dispatcher<work_thread, task> *dispatcher = singleton_template<task_dispatcher<work_thread, task>>::instance();
    dispatcher->init(m_threads); // number of work_thread
    // init the socket handler in epoll
    socket_handler *handler = singleton_template<socket_handler>::instance();
    handler->listen(m_ip, m_port);
    handler->handle(m_connects, m_wait_time); // main thread loop
}

void server::set_threads(size_t threads)
{
    m_threads = threads;
}

void server::set_connects(size_t connects)
{
    m_connects = connects;
}

void server::set_wait_time(size_t wait_time)
{
    m_wait_time = wait_time;
}

void server::config(const std::string &ip,
                    int port,
                    size_t threads,
                    size_t connects,
                    size_t wait_time)
{
    listen(ip, port);
    set_threads(threads);
    set_connects(connects);
    set_wait_time(wait_time);
}
