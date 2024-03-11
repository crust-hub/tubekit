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
#include "connection/http_connection.h"
#include "connection/stream_connection.h"
#include "connection/websocket_connection.h"
#include "connection/connection_mgr.h"
#include "task/http_task.h"
#include "task/stream_task.h"
#include "task/websocket_task.h"
#include "task/task_type.h"
#include "task/task_mgr.h"
#include "task/task_destory_impl.h"

using namespace std;
using namespace tubekit::server;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::thread;
using namespace tubekit::log;
using namespace tubekit::connection;
using namespace tubekit::task;

server::server() : m_ip("0.0.0.0"),
                   m_port(0),
                   m_threads(1024),
                   m_connects(1024),
                   m_wait_time(10)
{
}

server::~server()
{
    // release SSL_CTX
    if (get_use_ssl() && m_ssl_context)
    {
        SSL_CTX_free(m_ssl_context);
    }
}

void server::listen(const std::string &ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void server::start()
{
    LOG_ERROR("server::start ...");

    if (get_use_ssl())
    {
        LOG_ERROR("OpenSSL_version %s", OpenSSL_version(OPENSSL_VERSION));
        LOG_ERROR("SSLeay_version %s", SSLeay_version(SSLEAY_VERSION));
        SSL_library_init();
        SSL_load_error_strings();
        m_ssl_context = SSL_CTX_new(SSLv23_server_method());
        if (!m_ssl_context)
        {
            LOG_ERROR("SSL_CTX_new error");
            return;
        }
        SSL_CTX_set_options(m_ssl_context, SSL_OP_SINGLE_DH_USE);

        std::string crt_pem_path = get_crt_pem();
        int i_ret = SSL_CTX_use_certificate_file(m_ssl_context, crt_pem_path.c_str(), SSL_FILETYPE_PEM);
        if (1 != i_ret)
        {
            LOG_ERROR("SSL_CTX_use_certificate_file error: %s", ERR_error_string(ERR_get_error(), nullptr));
            return;
        }
        std::string key_pem_path = get_key_pem();
        i_ret = SSL_CTX_use_PrivateKey_file(m_ssl_context, key_pem_path.c_str(), SSL_FILETYPE_PEM);
        if (1 != i_ret)
        {
            LOG_ERROR("SSL_CTX_use_PrivateKey_file error: %s", ERR_error_string(ERR_get_error(), nullptr));
            return;
        }
    }

    // worker pool
    worker_pool *m_worker_pool = singleton<worker_pool>::instance();
    m_worker_pool->create(m_threads, new task_destory_impl());

    // socket object pool
    int iret = singleton<object_pool<socket::socket>>::instance()->init(m_connects, false);
    if (0 != iret)
    {
        LOG_ERROR("socket object_pool init return %d", iret);
        return;
    }

    // connection object pool and task object pool
    task::task_type task_type = get_task_type();
    switch (task_type)
    {
    case task::task_type::HTTP_TASK:
    {
        iret = singleton<object_pool<connection::http_connection>>::instance()->init(m_connects, false, nullptr);
        if (0 != iret)
        {
            LOG_ERROR("http_connection object_pool init return %d", iret);
            return;
        }
        iret = singleton<object_pool<task::http_task>>::instance()->init(m_connects * 3, true, nullptr);
        if (0 != iret)
        {
            LOG_ERROR("http_task object_pool init return %d", iret);
            return;
        }
        break;
    }
    case task::task_type::STREAM_TASK:
    {
        iret = singleton<object_pool<connection::stream_connection>>::instance()->init(m_connects, false, nullptr);
        if (0 != iret)
        {
            LOG_ERROR("stream_connection object_pool init return %d", iret);
            return;
        }
        iret = singleton<object_pool<task::stream_task>>::instance()->init(m_connects * 3, true, nullptr);
        if (0 != iret)
        {
            LOG_ERROR("stream_task object_pool init return %d", iret);
            return;
        }
        break;
    }
    case task::task_type::WEBSOCKET_TASK:
    {
        iret = singleton<object_pool<connection::websocket_connection>>::instance()->init(m_connects, false, nullptr);
        if (0 != iret)
        {
            LOG_ERROR("websocket_connection object_pool init return %d", iret);
            return;
        }
        iret = singleton<object_pool<task::websocket_task>>::instance()->init(m_connects * 3, true, nullptr);
        if (0 != iret)
        {
            LOG_ERROR("websocket_task object_pool init return %d", iret);
            return;
        }
        break;
    }
    default:
    {
        LOG_ERROR("not found task::task_type");
        return;
    }
    }

    // connection_mgr
    iret = singleton<connection::connection_mgr>::instance()->init(task_type);
    if (0 != iret)
    {
        LOG_ERROR("connection_mgr init return %d", iret);
        return;
    }

    // task_mgr
    iret = singleton<task::task_mgr>::instance()->init(task_type);
    if (0 != iret)
    {
        LOG_ERROR("task_mgr init return %d", iret);
        return;
    }

    socket_handler *handler = singleton<socket_handler>::instance();
    iret = handler->init(m_ip, m_port, m_connects, m_wait_time);
    if (0 != iret)
    {
        LOG_ERROR("socket_handler init return %d", iret);
        return;
    }

    handler->handle(); // main thread event loop

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

void server::set_http_static_dir(std::string http_static_dir)
{
    m_http_static_dir = http_static_dir;
}

const std::string &server::get_http_static_dir()
{
    return m_http_static_dir;
}

void server::set_lua_dir(std::string lua_dir)
{
    m_lua_dir = lua_dir;
}

const std::string &server::get_lua_dir()
{
    return m_lua_dir;
}

void server::set_daemon(bool daemon)
{
    m_daemon = daemon;
}

enum tubekit::task::task_type server::get_task_type()
{
    if (m_task_type == "HTTP_TASK")
    {
        return HTTP_TASK;
    }
    else if (m_task_type == "STREAM_TASK")
    {
        return STREAM_TASK;
    }
    else if (m_task_type == "WEBSOCKET_TASK")
    {
        return WEBSOCKET_TASK;
    }
    return NONE;
}

void server::set_use_ssl(bool use_ssl)
{
    m_use_ssl = use_ssl;
}

void server::set_crt_pem(std::string crt_pem)
{
    m_crt_pem = crt_pem;
}

void server::set_key_pem(std::string key_pem)
{
    m_key_pem = key_pem;
}

bool server::get_use_ssl()
{
    return m_use_ssl;
}

std::string server::get_crt_pem()
{
    return m_crt_pem;
}

std::string server::get_key_pem()
{
    return m_key_pem;
}

void server::config(const std::string &ip,
                    int port,
                    size_t threads,
                    size_t connects,
                    size_t wait_time,
                    std::string task_type,
                    std::string http_static_dir,
                    std::string lua_dir,
                    bool daemon, /*= false*/
                    std::string crt_pem /*= ""*/,
                    std::string key_pem /*= ""*/,
                    bool use_ssl /*= false*/)
{
    listen(ip, port);
    set_threads(threads);
    set_connects(connects);
    set_wait_time(wait_time);
    set_task_type(task_type);
    set_http_static_dir(http_static_dir);
    set_lua_dir(lua_dir);

    set_daemon(daemon);
    set_use_ssl(use_ssl);
    set_crt_pem(crt_pem);
    set_key_pem(key_pem);
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

SSL_CTX *server::get_ssl_ctx()
{
    return m_ssl_context;
}
