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

    // recv data
    if (t_stream_connection->connection_state == stream_connection::state::WAIT_RECV)
    {
        // can read
        if (reason_recv)
        {
            t_stream_connection->connection_state = stream_connection::state::RECVING;
            // read data from socket to connection layer buffer
            t_stream_connection->sock2buf();
            t_stream_connection->connection_state = stream_connection::state::WAIT_PROCESS;
            handler->attach(socket_ptr, true);
        }
        else
        {
            t_stream_connection->connection_state = stream_connection::state::WAIT_RECV;
            handler->attach(socket_ptr);
        }
        return;
    }

    // process data
    if (t_stream_connection->connection_state == stream_connection::state::WAIT_PROCESS)
    {
        t_stream_connection->connection_state = stream_connection::state::PROCESSING;
        // processing data in connection layer,judge continue recv or process protocol pack
        // write response data to send buffer
        t_stream_connection->m_send_buffer.write("hello tubekit", 14);
        t_stream_connection->connection_state = stream_connection::state::WAIT_SEND;
        handler->attach(socket_ptr, true);
        return;
    }

    // send data
    if (t_stream_connection->connection_state == stream_connection::state::WAIT_SEND)
    {
        t_stream_connection->connection_state = stream_connection::state::SENDING;

        // can send
        if (reason_send)
        {
            // send data to socket from connection layer
            bool bRet = t_stream_connection->buf2sock();
            if (bRet) // continue send
            {
                t_stream_connection->connection_state = stream_connection::state::WAIT_SEND;
                handler->attach(socket_ptr, true);
            }
            else
            {
                // trigger send end callback
                t_stream_connection->connection_state = stream_connection::state::WAIT_RECV;
                handler->attach(socket_ptr);
            }
        }
        else
        {
            t_stream_connection->connection_state = stream_connection::state::NONE;
            handler->attach(socket_ptr, true);
        }

        return;
    }

    if (t_stream_connection->connection_state == stream_connection::state::NONE)
    {
        handler->remove(socket_ptr);
        return;
    }

    handler->attach(socket_ptr);
}
