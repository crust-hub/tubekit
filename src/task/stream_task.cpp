#include "task/stream_task.h"

#include <string>
#include <sstream>
#include <memory.h>
#include <vector>

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

    if (!socket_ptr->close_callback)
    {
        socket_ptr->close_callback = [socket_ptr]()
        {
            singleton<connection_mgr>::instance()->remove(socket_ptr);
        };
    }

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
    if (t_stream_connection->connection_state == stream_connection::state::RECV)
    {
        if (reason_recv)
        {
            // can read
            t_stream_connection->connection_state = stream_connection::state::PROCESS;
        }
        handler->attach(socket_ptr);
        return;
    }

    // process data
    if (t_stream_connection->connection_state == stream_connection::state::PROCESS)
    {
        t_stream_connection->connection_state = stream_connection::state::SEND;
        handler->attach(socket_ptr);
        return;
    }

    // send data
    if (t_stream_connection->connection_state == stream_connection::state::SEND)
    {
        handler->attach(socket_ptr); // handler->remove(socket_ptr);
        return;
    }

    handler->remove(socket_ptr); // handler->remove(socket_ptr);

    // // exceute workflow
    // workflow *workflow_instance = singleton<workflow>::instance();

    // ostringstream os;
    // os << (int)(msg_head.cmd);
    // const std::string work = os.str();
    // const std::string input(buf);
    // std::string output;
    // // send body to work
    // workflow_instance->run(cmd, input, output);
    // // response client
    // socketfd->send(output.c_str(), output.length());
    // // return to epoll for next time
    // handler->attach(socketfd);
}
