#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tubekit-log/logger.h>

#include "server/server.h"
#include "thread/worker.h"
#include "thread/task.h"
#include "thread/worker_pool.h"
#include "socket/socket_handler.h"
#include "utility/singleton.h"

using namespace std;
using namespace tubekit::server;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::thread;
using namespace tubekit::log;

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
    std::cout << "server start..." << std::endl;

    // worker pool
    worker_pool *m_worker_pool = singleton<worker_pool>::instance();
    m_worker_pool->create(m_threads);

    // init the socket handler in epoll
    socket_handler *handler = singleton<socket_handler>::instance();
    handler->listen(m_ip, m_port);
    handler->handle(m_connects, m_wait_time); // main thread loop

    LOG_ERROR("socket_handler handle return");
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

void server::set_task_type(std::string task_type)
{
    m_task_type = task_type;
}

void server::set_daemon(bool daemon)
{
    m_daemon = daemon;
}

enum server::TaskType server::get_task_type()
{
    if (m_task_type == "HTTP_TASK")
    {
        return HTTP_TASK;
    }
    else if (m_task_type == "STREAM_TASK")
    {
        return STREAM_TASK;
    }
    return NONE;
}

void server::config(const std::string &ip,
                    int port,
                    size_t threads,
                    size_t connects,
                    size_t wait_time,
                    std::string task_type,
                    bool daemon)
{
    listen(ip, port);
    set_threads(threads);
    set_connects(connects);
    set_wait_time(wait_time);
    set_task_type(task_type);
    set_daemon(daemon);
}

bool server::is_stop()
{
    return stop_flag;
}

bool server::on_stop()
{
    if (stop_flag)
    {
        singleton<worker_pool>::instance()->stop();
        return true; // main process close
    }
    return false;
}

void server::to_stop()
{
    stop_flag = true;
}