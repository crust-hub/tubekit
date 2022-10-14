#include <string>
#include <sstream>

#include "work_task.h"
#include "socket/socket_handler.h"
#include "utility/singleton_template.h"
#include "engine/workflow.h"
#include "engine/workflow.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::engine;

work_task::work_task(tubekit::socket::socket *m_socket) : task(m_socket)
{
}

work_task::~work_task()
{
}

void work_task::destroy()
{
    delete this;
}

void work_task::run()
{
    socket_handler *handler = singleton_template<socket_handler>::instance();
    socket::socket *socketfd = static_cast<socket::socket *>(m_data);
    work_task_msg_head msg_head;
    memset(&msg_head, 0, sizeof(msg_head));
    int len = socketfd->recv((char *)(&msg_head), sizeof(msg_head)); //取出消息头
    if (len == 0)
    {
        handler->remove(socketfd);
        return;
    }
    if (len == -1 && errno == EAGAIN)
    {
        handler->attach(socketfd);
        return;
    }
    if (len == -1 && errno == EWOULDBLOCK)
    {
        handler->attach(socketfd);
        return;
    }
    if (len == -1 && errno == EINTR)
    {
        handler->attach(socketfd);
        return;
    }
    if (len != sizeof(msg_head))
    {
        handler->remove(socketfd);
        return;
    }
    if (strcmp(msg_head.flag, "work") != 0)
    {
        handler->remove(socketfd);
        return;
    }
    if (msg_head.len >= uint32_t(recv_buffer_size))
    {
        handler->remove(socketfd);
        return;
    }
    char buf[recv_buffer_size];
    memset(buf, 0, recv_buffer_size);
    len = socketfd->recv(buf, msg_head.len);
    if (len == -1 && errno == EAGAIN)
    {
        handler->remove(socketfd);
        return;
    }
    if (len == -1 && errno == EWOULDBLOCK)
    {
        handler->remove(socketfd);
        return;
    }
    if (len == -1 && errno == EINTR)
    {
        handler->remove(socketfd);
        return;
    }
    if (len != (int)(msg_head.len))
    {
        handler->remove(socketfd);
        return;
    }

    workflow *workflow_instance = singleton_template<workflow>::instance();

    ostringstream os;
    os << (int)(msg_head.cmd);
    const std::string work = os.str();
    const std::string input(buf);
    std::string output;
    workflow_instance->run(work, input, output);
    socketfd->send(output.c_str(), output.length());
    handler->attach(socketfd);
}