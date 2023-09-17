#include "connection/connection_mgr.h"
#include "thread/auto_lock.h"

using tubekit::connection::connection;
using tubekit::connection::connection_mgr;
using tubekit::connection::http_connection;
using tubekit::connection::stream_connection;

connection_mgr::connection_mgr()
{
}

connection_mgr::~connection_mgr()
{
}

bool connection_mgr::add(void *index_ptr, connection *conn_ptr)
{
    tubekit::thread::auto_lock lock(m_mutex);
    auto res = m_map.find(index_ptr);
    if (res != m_map.end() && res->second == conn_ptr)
    {
        return true;
    }
    if (res != m_map.end())
    {
        return false;
    }
    m_map.insert({index_ptr, conn_ptr});
    return true;
}

bool connection_mgr::remove(void *index_ptr)
{
    tubekit::thread::auto_lock lock(m_mutex);
    auto res = m_map.find(index_ptr);
    if (res != m_map.end())
    {
        res->second->close_before();
        // triger delete connection
        delete res->second;
        m_map.erase(res);
        return true;
    }
    return false;
}

bool connection_mgr::has(void *index_ptr)
{
    tubekit::thread::auto_lock lock(m_mutex);
    auto res = m_map.find(index_ptr);
    if (res != m_map.end())
    {
        return true;
    }
    return false;
}

connection *connection_mgr::get(void *index_ptr)
{
    tubekit::thread::auto_lock lock(m_mutex);
    auto res = m_map.find(index_ptr);
    if (res != m_map.end())
    {
        return res->second;
    }
    return nullptr;
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