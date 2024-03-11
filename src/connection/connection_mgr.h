#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <functional>
#include "thread/mutex.h"
#include "connection/connection.h"
#include "connection/http_connection.h"
#include "connection/stream_connection.h"
#include "connection/websocket_connection.h"
#include "task/task_type.h"

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
        bool mark_close(void *index_ptr);
        void for_each(std::function<void(connection &conn)> callback);

        /**
         * @brief Sending data to a stream connection can prevent the problem
         *        of using a null pointer in the connection.
         *        In addition to processing the allocated connection
         *        within the worker thread, safe_send should be used
         *
         * @param index_ptr
         * @param buffer
         * @param len
         * @return true
         * @return false
         */
        bool safe_send(void *index_ptr, const char *buffer, size_t len);

        void on_new_connection(void *index_ptr);

        int init(tubekit::task::task_type task_type);

    public:
        connection *create();
        void release(connection *connection_ptr);

    public:
        static http_connection *convert_to_http(connection *conn_ptr);
        static stream_connection *convert_to_stream(connection *conn_ptr);
        websocket_connection *convert_to_websocket(connection *conn_ptr);
        static bool is_http(connection *conn_ptr);
        static bool is_stream(connection *conn_ptr);
        static bool is_websocket(connection *conn_ptr);

    private:
        std::unordered_map<void *, connection *> m_map;
        tubekit::thread::mutex m_mutex;
        tubekit::task::task_type m_task_type;
    };
}