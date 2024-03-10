#include "task/stream_task.h"

#include <string>
#include <sstream>
#include <memory.h>
#include <vector>
#include <tubekit-log/logger.h>

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

stream_task::stream_task(tubekit::socket::socket *m_socket) : task(m_socket),
                                                              reason_send(false),
                                                              reason_recv(false)
{
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
    if (nullptr == m_data)
    {
        return;
    }
    socket::socket *socket_ptr = static_cast<socket::socket *>(m_data);

    // bind socket close callback
    if (!socket_ptr->close_callback)
    {
        socket_ptr->close_callback = [socket_ptr]() {
        };
    }

    // get connection layer instance
    connection::stream_connection *t_stream_connection = (connection::stream_connection *)singleton<connection_mgr>::instance()->get(socket_ptr);

    // connection is close
    if (nullptr == t_stream_connection || t_stream_connection->is_close())
    {
        // here, make sure closing connection and socket once
        if (t_stream_connection)
        {
            singleton<connection_mgr>::instance()->remove(socket_ptr);
            singleton<socket_handler>::instance()->push_wait_remove(socket_ptr);
        }
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
            singleton<connection_mgr>::instance()->on_new_connection(socket_ptr);
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
                singleton<connection_mgr>::instance()->mark_close(socket_ptr); // final connection and socket
                return;
            }
        }
    }

    // recv data
    {
        // read data from socket to connection layer buffer
        if (false == t_stream_connection->sock2buf())
        {
            singleton<connection_mgr>::instance()->mark_close(socket_ptr);
        }
    }

    // process data
    {
        stream_app::process_connection(*t_stream_connection);
    }

    bool b_send = false;

    // send data
    {
        // send data to socket from connection layer
        b_send = t_stream_connection->buf2sock();
    }

    int i_ret = singleton<socket_handler>::instance()->attach(socket_ptr, b_send);
    if (i_ret != 0)
    {
        LOG_ERROR("socket handler attach error %d", i_ret);
    }
    return;
}
