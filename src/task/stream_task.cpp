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
    socket_handler *handler = singleton<socket_handler>::instance();
    socket::socket *socket_ptr = static_cast<socket::socket *>(m_data);
    // socket object free hook function
    if (!socket_ptr->delete_ptr_hook)
    {
        socket_ptr->delete_ptr_hook = [](void *ptr)
        {
            if (ptr)
            {
                connection::stream_connection *t_stream_connection = (connection::stream_connection *)ptr;
                delete t_stream_connection;
            }
        };
    }
    // binding stream_connection for socket
    if (nullptr == socket_ptr->ptr)
    {
        socket_ptr->ptr = new connection::stream_connection(socket_ptr->get_fd());
    }

    connection::stream_connection *t_stream_connection = static_cast<connection::stream_connection *>(socket_ptr->ptr);

    // recv data
    if (t_stream_connection->connection_state == stream_connection::state::RECV)
    {
        if (reason_recv)
        {
            // can read
            t_stream_connection->connection_state = stream_connection::state::PROCESS;
        }
        handler->attach(socket_ptr, true);
        return;
    }

    // process data
    if (t_stream_connection->connection_state == stream_connection::state::PROCESS)
    {
        t_stream_connection->connection_state = stream_connection::state::SEND;
        handler->attach(socket_ptr, true);
        return;
    }

    // send data
    if (t_stream_connection->connection_state == stream_connection::state::SEND)
    {
        handler->attach(socket_ptr); // handler->remove(socket_ptr);
        return;
    }

    handler->attach(socket_ptr); // handler->remove(socket_ptr);

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
