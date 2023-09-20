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

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::connection;
using namespace tubekit::app;

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

    // Connection Mgr
    if (false == singleton<connection_mgr>::instance()->has(socket_ptr))
    {
        LOG_ERROR("!singleton<connection_mgr>::instance()->has(socket_ptr)");
        singleton<socket_handler>::instance()->remove(socket_ptr);
        return;
    }

    // get connection layer instance
    connection::stream_connection *t_stream_connection = (connection::stream_connection *)singleton<connection_mgr>::instance()->get(socket_ptr);

    // connection is close
    if (nullptr == t_stream_connection || t_stream_connection->is_close())
    {
        singleton<socket_handler>::instance()->remove(socket_ptr);
        singleton<connection_mgr>::instance()->remove(socket_ptr);
        return;
    }

    // recv data
    bool b_recv = false;
    {
        // read data from socket to connection layer buffer
        b_recv = t_stream_connection->sock2buf();
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

    if (b_send)
    {
        singleton<socket_handler>::instance()->attach(socket_ptr, true);
        return;
    }

    singleton<socket_handler>::instance()->attach(socket_ptr);
}
