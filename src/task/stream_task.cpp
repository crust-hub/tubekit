#include "task/stream_task.h"

#include <string>
#include <sstream>
#include <memory.h>
#include <vector>
#include <tubekit-log/logger.h>
#include <stdexcept>

#include "socket/socket_handler.h"
#include "utility/singleton.h"
#include "connection/stream_connection.h"
#include "connection/connection_mgr.h"
#include "connection/connection.h"
#include "app/stream_app.h"
#include "server/server.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::connection;
using namespace tubekit::app;
using namespace tubekit::server;

stream_task::stream_task(uint64_t gid) : task(gid)
{
    reason_send = false;
    reason_recv = false;
}

stream_task::~stream_task()
{
    destroy();
}

void stream_task::destroy()
{
}

void stream_task::run()
{
    if (0 == get_gid())
    {
        return;
    }
    socket::socket *socket_ptr = nullptr;
    connection::connection *conn_ptr = nullptr;
    bool found = false;

    singleton<connection_mgr>::instance()->if_exist(
        get_gid(),
        [&socket_ptr, &conn_ptr, &found](uint64_t key, std::pair<tubekit::socket::socket *, tubekit::connection::connection *> value)
        {
            socket_ptr = value.first;
            conn_ptr = value.second;
            found = true;
        },
        nullptr);

    if (false == found)
    {
        return;
    }

    // bind socket close callback
    if (!socket_ptr->close_callback)
    {
        socket_ptr->close_callback = [socket_ptr]() {
        };
    }

    // get connection layer instance
    connection::stream_connection *t_stream_connection = (connection::stream_connection *)conn_ptr;

    // connection is close
    if (t_stream_connection->is_close())
    {
        // here, make sure closing connection and socket once
        singleton<connection_mgr>::instance()->remove(
            get_gid(),
            [](uint64_t key, std::pair<tubekit::socket::socket *, tubekit::connection::connection *> value)
            {
                singleton<connection_mgr>::instance()->release(value.second);
                singleton<socket_handler>::instance()->push_wait_remove(value.first);
            },
            nullptr);
        return;
    }

    // ssl
    if (singleton<server::server>::instance()->get_use_ssl() && !socket_ptr->get_ssl_accepted() && !t_stream_connection->is_close())
    {
        int ssl_status = SSL_accept(socket_ptr->get_ssl_instance());

        if (1 == ssl_status)
        {
            // LOG_ERROR("set_ssl_accepted(true)");
            socket_ptr->set_ssl_accepted(true);
            // triger new connection hook
            singleton<connection_mgr>::instance()->on_new_connection(get_gid());
        }
        else if (0 == ssl_status)
        {
            // LOG_ERROR("SSL_accept ssl_status == 0");
            // need more data or space
            singleton<socket_handler>::instance()->attach(socket_ptr, true);
            return;
        }
        else
        {
            int ssl_error = SSL_get_error(socket_ptr->get_ssl_instance(), ssl_status);
            if (ssl_error == SSL_ERROR_WANT_READ)
            {
                // need more data or space
                singleton<socket_handler>::instance()->attach(socket_ptr);
                return;
            }
            else if (ssl_error == SSL_ERROR_WANT_WRITE)
            {
                singleton<socket_handler>::instance()->attach(socket_ptr, true);
                return;
            }
            else
            {
                LOG_ERROR("SSL_accept ssl_status[%d] error: %s", ssl_status, ERR_error_string(ERR_get_error(), nullptr));
                singleton<connection_mgr>::instance()->mark_close(get_gid()); // final connection and socket
                return;
            }
        }
    }

    // recv data
    {
        // read data from socket to connection layer buffer
        bool need_task = false;
        bool sock2buf_res = t_stream_connection->sock2buf(need_task);
        if (false == sock2buf_res)
        {
            if (false == need_task)
            {
                t_stream_connection->mark_close();
                return;
            }
            else
            {
                singleton<socket_handler>::instance()->do_task(get_gid(), true, true);
            }
        }
    }

    // process data
    {
        try
        {
            stream_app::process_connection(*t_stream_connection);
        }
        catch (std::exception &e)
        {
            LOG_ERROR(e.what());
            t_stream_connection->mark_close();
            return;
        }
    }

    bool b_send = false, b_closed = false;

    // send data
    {
        // send data to socket from connection layer
        b_send = t_stream_connection->buf2sock(b_closed);
        if (b_closed)
        {
            t_stream_connection->mark_close();
        }
    }

    int i_ret = singleton<socket_handler>::instance()->attach(socket_ptr, b_send);
    if (i_ret != 0)
    {
        LOG_ERROR("socket handler attach error %d", i_ret);
    }
    return;
}
