#pragma once

#include "thread/task.h"
#include "socket/socket.h"
#include "task/stream_task.h"
#include "task/http_task.h"

namespace tubekit
{
    namespace task
    {
        class task_factory
        {
        public:
            enum task_type
            {
                STREAM_TASK = 0, // workflow engine
                HTTP_TASK        // http engine
            };
            static tubekit::thread::task *create(tubekit::socket::socket *m_socket, task_type type)
            {
                switch (type)
                {
                case STREAM_TASK:
                    return new stream_task(m_socket);
                    break;
                case HTTP_TASK:
                    return new http_task(m_socket);
                default:
                    break;
                }
                return nullptr;
            }
        };
    }
}