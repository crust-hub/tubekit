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
#include "socket/socket.h"

namespace tubekit::connection
{
    class safe_mapping
    {
    private:
        friend class connection_mgr;
        safe_mapping();
        ~safe_mapping();
        void if_exist(uint64_t gid,
                      std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                      std::function<void(uint64_t)> failed_callback);

        void remove(uint64_t gid,
                    std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                    std::function<void(uint64_t)> failed_callback);

        void insert(uint64_t gid,
                    std::pair<socket::socket *, connection *> value,
                    std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                    std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> failed_callback);

        std::unordered_map<uint64_t, std::pair<socket::socket *, connection *>> gid2pair{};
        tubekit::thread::mutex lock;
    };

    class connection_mgr
    {
    public:
        connection_mgr();
        ~connection_mgr();

        // need lock method
    public:
        void if_exist(uint64_t gid,
                      std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                      std::function<void(uint64_t)> failed_callback);

        void remove(uint64_t gid,
                    std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                    std::function<void(uint64_t)> failed_callback);

        void insert(uint64_t gid,
                    std::pair<socket::socket *, connection *> value,
                    std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> succ_callback,
                    std::function<void(uint64_t, std::pair<socket::socket *, connection *>)> failed_callback);
        void on_new_connection(uint64_t gid);
        void mark_close(uint64_t gid);

        safe_mapping *m_safe_mapping{nullptr};
        uint32_t m_thread_size{0};

    public:
        int init(tubekit::task::task_type task_type, uint32_t thread_size);

    public:
        connection *create();
        void release(connection *connection_ptr);

    public:
        static http_connection *convert_to_http(connection *conn_ptr);
        static stream_connection *convert_to_stream(connection *conn_ptr);
        static websocket_connection *convert_to_websocket(connection *conn_ptr);
        static bool is_http(connection *conn_ptr);
        static bool is_stream(connection *conn_ptr);
        static bool is_websocket(connection *conn_ptr);

    private:
        tubekit::task::task_type m_task_type{tubekit::task::task_type::NONE};
        tubekit::thread::mutex m_mutex;
    };
}