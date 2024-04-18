#include "connection/connection_mgr.h"
#include "thread/auto_lock.h"
#include "app/stream_app.h"
#include "app/websocket_app.h"
#include "task/task_type.h"
#include "utility/object_pool.h"
#include "utility/singleton.h"

#include <tubekit-log/logger.h>
#include <stdexcept>

using tubekit::app::stream_app;
using tubekit::app::websocket_app;
using tubekit::connection::connection;
using tubekit::connection::connection_mgr;
using tubekit::connection::http_connection;
using tubekit::connection::safe_mapping;
using tubekit::connection::stream_connection;
using tubekit::connection::websocket_connection;
using tubekit::task::task_type;
using tubekit::utility::object_pool;
using tubekit::utility::singleton;
using tubekit::thread::auto_lock;

safe_mapping::safe_mapping()
{
}

safe_mapping::~safe_mapping()
{
}

void safe_mapping::if_exist(uint64_t gid,
                            std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                            std::function<void(uint64_t)> failed_callback)
{
    auto_lock raiilock(lock);
    auto iter = gid2pair.find(gid);
    if (iter != gid2pair.end())
    {
        if (succ_callback)
        {
            succ_callback(iter->first, iter->second);
        }
    }
    else
    {
        if (failed_callback)
        {
            failed_callback(gid);
        }
    }
}

void safe_mapping::remove(uint64_t gid,
                          std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                          std::function<void(uint64_t)> failed_callback)
{
    auto_lock raiilock(lock);
    auto iter = gid2pair.find(gid);
    if (iter != gid2pair.end())
    {
        auto first = iter->first;
        auto second = iter->second;
        gid2pair.erase(iter);
        if (succ_callback)
        {
            succ_callback(first, second);
        }
    }
    else
    {
        if (failed_callback)
        {
            failed_callback(gid);
        }
    }
}

void safe_mapping::insert(uint64_t gid,
                          std::pair<socket::socket *, connection *> value,
                          std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                          std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> failed_callback)
{
    auto_lock raiilock(lock);
    auto iter = gid2pair.find(gid);
    if (iter != gid2pair.end())
    {
        if (failed_callback)
        {
            failed_callback(gid, value);
        }
    }
    else
    {
        gid2pair[gid] = value;
        if (succ_callback)
        {
            succ_callback(gid, value);
        }
    }
}

void connection_mgr::if_exist(uint64_t gid,
                              std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                              std::function<void(uint64_t)> failed_callback)
{
    uint64_t hash_idx = gid % m_thread_size;
    m_safe_mapping[hash_idx].if_exist(gid, succ_callback, failed_callback);
}

void connection_mgr::remove(uint64_t gid,
                            std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                            std::function<void(uint64_t)> failed_callback)
{
    uint64_t hash_idx = gid % m_thread_size;
    m_safe_mapping[hash_idx].remove(
        gid,
        [&succ_callback](uint64_t key, std::pair<socket::socket *, connection *> value)
        {
            try
            {
                if (is_stream(value.second))
                {
                    stream_app::on_close_connection(*convert_to_stream(value.second));
                }
                if (is_websocket(value.second))
                {
                    websocket_app::on_close_connection(*convert_to_websocket(value.second));
                }
                succ_callback(key, value);
            }
            catch (const std::exception &e)
            {
                LOG_ERROR(e.what());
            }
        },
        failed_callback);
}

void connection_mgr::insert(uint64_t gid,
                            std::pair<socket::socket *, connection *> value,
                            std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                            std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> failed_callback)
{
    uint64_t hash_idx = gid % m_thread_size;
    m_safe_mapping[hash_idx].insert(gid, value, succ_callback, failed_callback);
}

connection_mgr::connection_mgr()
{
}

connection_mgr::~connection_mgr()
{
    if (m_safe_mapping)
    {
        delete[] m_safe_mapping;
    }
}

void connection_mgr::on_new_connection(uint64_t gid)
{
    if_exist(
        gid,
        [](uint64_t key, std::pair<tubekit::socket::socket *, tubekit::connection::connection *> value)
        {
            try
            {
                if (is_stream(value.second))
                {
                    stream_app::on_new_connection(*convert_to_stream(value.second));
                }
                if (is_websocket(value.second))
                {
                    websocket_app::on_new_connection(*convert_to_websocket(value.second));
                }
            }
            catch (const std::exception &e)
            {
                LOG_ERROR(e.what());
            }
        },
        nullptr);
}

void connection_mgr::mark_close(uint64_t gid)
{
    if_exist(
        gid,
        [](uint64_t key, std::pair<tubekit::socket::socket *, tubekit::connection::connection *> value)
        {
            value.second->mark_close();
        },
        nullptr);
}

http_connection *connection_mgr::convert_to_http(connection *conn_ptr)
{
    if (nullptr == conn_ptr)
    {
        return nullptr;
    }
    if (is_http(conn_ptr))
    {
        return (http_connection *)conn_ptr;
    }
    return nullptr;
}

stream_connection *connection_mgr::convert_to_stream(connection *conn_ptr)
{
    if (nullptr == conn_ptr)
    {
        return nullptr;
    }
    if (is_stream(conn_ptr))
    {
        return (stream_connection *)conn_ptr;
    }
    return nullptr;
}

websocket_connection *connection_mgr::convert_to_websocket(connection *conn_ptr)
{
    if (nullptr == conn_ptr)
    {
        return nullptr;
    }
    if (is_websocket(conn_ptr))
    {
        return (websocket_connection *)conn_ptr;
    }
    return nullptr;
}

bool connection_mgr::is_http(connection *conn_ptr)
{
    if (nullptr == conn_ptr)
    {
        return false;
    }
    if (typeid(*conn_ptr) == typeid(http_connection))
    {
        return true;
    }
    return false;
}

bool connection_mgr::is_stream(connection *conn_ptr)
{
    if (nullptr == conn_ptr)
    {
        return false;
    }
    if (typeid(*conn_ptr) == typeid(stream_connection))
    {
        return true;
    }
    return false;
}

bool connection_mgr::is_websocket(connection *conn_ptr)
{
    if (nullptr == conn_ptr)
    {
        return false;
    }
    if (typeid(*conn_ptr) == typeid(websocket_connection))
    {
        return true;
    }
    return false;
}

int connection_mgr::init(tubekit::task::task_type task_type, uint32_t thread_size)
{
    m_task_type = task_type;
    m_thread_size = thread_size;
    m_safe_mapping = new safe_mapping[thread_size];
    return 0;
}

void connection_mgr::release(connection *connection_ptr)
{
    if (!connection_ptr)
    {
        return;
    }
    switch (m_task_type)
    {
    case task_type::STREAM_TASK:
    {
        singleton<object_pool<stream_connection>>::instance()->release(dynamic_cast<stream_connection *>(connection_ptr));
        // LOG_DEBUG("connection space:%d", singleton<object_pool<stream_connection>>::instance()->space());
        break;
    }
    case task_type::HTTP_TASK:
    {
        singleton<object_pool<http_connection>>::instance()->release(dynamic_cast<http_connection *>(connection_ptr));
        // LOG_DEBUG("connection space:%d", singleton<object_pool<http_connection>>::instance()->space());
        break;
    }
    case task_type::WEBSOCKET_TASK:
    {
        singleton<object_pool<websocket_connection>>::instance()->release(dynamic_cast<websocket_connection *>(connection_ptr));
        // LOG_DEBUG("connection space:%d", singleton<object_pool<websocket_connection>>::instance()->space());
        break;
    }
    default:
        break;
    }
}

connection *connection_mgr::create()
{
    connection *p_connection = nullptr;
    switch (m_task_type)
    {
    case task_type::STREAM_TASK:
    {
        p_connection = singleton<object_pool<stream_connection>>::instance()->allocate();
        break;
    }
    case task_type::HTTP_TASK:
    {
        p_connection = singleton<object_pool<http_connection>>::instance()->allocate();
        break;
    }
    case task_type::WEBSOCKET_TASK:
    {
        p_connection = singleton<object_pool<websocket_connection>>::instance()->allocate();
        break;
    }
    default:
        break;
    }
    return p_connection;
}