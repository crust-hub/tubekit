#include "task/stream_task.h"

#include <string>
#include <sstream>
#include <memory.h>
#include <vector>
#include <tubekit-log/logger.h>

#include "socket/socket_handler.h"
#include "utility/singleton.h"
#include "engine/workflow.h"
#include "engine/workflow.h"
#include "connection/stream_connection.h"
#include "connection/connection_mgr.h"
#include "connection/connection.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::engine;
using namespace tubekit::connection;

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
    socket_handler *handler = singleton<socket_handler>::instance();
    socket::socket *socket_ptr = static_cast<socket::socket *>(m_data);

    // bind socket close callback
    if (!socket_ptr->close_callback)
    {
        socket_ptr->close_callback = [socket_ptr]()
        {
            singleton<connection_mgr>::instance()->remove(socket_ptr);
        };
    }

    // Connection Mgr
    if (!singleton<connection_mgr>::instance()->has(socket_ptr))
    {
        connection::stream_connection *t_stream_connection = new connection::stream_connection(socket_ptr);
        if (nullptr == t_stream_connection)
        {
            handler->remove(socket_ptr);
            return;
        }
        bool add_res = singleton<connection_mgr>::instance()->add(socket_ptr, t_stream_connection);
        if (!add_res)
        {
            delete t_stream_connection;
            return;
        }
    }

    connection::stream_connection *t_stream_connection = (connection::stream_connection *)singleton<connection_mgr>::instance()->get(socket_ptr);

    if (nullptr == t_stream_connection)
    {
        handler->remove(socket_ptr);
        return;
    }

    handler->remove(socket_ptr);
    return;

    // // recv data
    // {
    //     // can read
    //     // read data from socket to connection layer buffer
    //     bool b_ret = t_stream_connection->sock2buf();
    // }

    // // process data
    // {
    // // process pack
    //     try
    //     {
    //         t_stream_connection->send("hello tubekit", 14);
    //     }
    //     catch (...)
    //     {
    //     }
    // }

    // // send data
    // {
    //     // send data to socket from connection layer
    //     bool bRet = t_stream_connection->buf2sock();
    // }
}
