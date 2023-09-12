#pragma once

#include <map>
// #include <unordered_map>
#include <vector>
#include "thread/mutex.h"
#include "connection/connection.h"
#include "connection/http_connection.h"
#include "connection/stream_connection.h"

namespace tubekit::connection
{
    class connection_mgr
    {
    public:
        connection_mgr();
        ~connection_mgr();
        bool add(void *index_ptr, connection *conn_ptr);
        bool remove(void *index_ptr);
        bool has(void *index_ptr);
        connection *get(void *index_ptr);

    public:
        static http_connection *convert_to_http(connection *conn_ptr);
        static stream_connection *convert_to_stream(connection *conn_ptr);
        static bool is_http(connection *conn_ptr);
        static bool is_stream(connection *conn_ptr);

    private:
        std::map<void *, connection *> m_map;
        // std::unordered_multimap<void *, std::vector<char>> m_wait_send_data;
        tubekit::thread::mutex m_mutex;
    };
}