#pragma once

#include "thread/task.h"
#include "socket/socket.h"
#include "work_task.h"

namespace tubekit
{
    namespace task
    {
        class task_factory
        {
        public:
            enum task_type
            {
                WORK_TASK = 0
            };
            static tubekit::thread::task *create(tubekit::socket::socket *m_socket, task_type type)
            {
                switch (type)
                {
                case WORK_TASK:
                    return new work_task(m_socket);
                    break;
                default:
                    break;
                }
                return nullptr;
            }
        };
    }
}