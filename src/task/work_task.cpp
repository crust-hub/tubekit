#include "work_task.h"
#include "socket/socket_handler.h"
#include "utility/singleton_template.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;

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
    /*TODO...*/
}