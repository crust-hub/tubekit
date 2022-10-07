#pragma once
#include <stddef.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string>
#include <cstring>

namespace tubekit
{
    namespace socket
    {
        class event_poller
        {
        public:
            /**
             * @brief Construct a new event poller object
             *
             * @param et 是否使用EPOLLET模式，默认为EPOLLLT模式
             */
            event_poller(bool et = true);
            ~event_poller();
            /**
             * @brief config epfd and creating events array
             *
             * @param max_connections
             */
            void create(int max_connections);
            /**
             * @brief 增加需要监听的句柄
             *
             * @param fd 需要监听的句柄
             * @param ptr 让相应的event_poll.data.ptr携带的数据
             * @param events EPOLLIN|EPOLOUT
             */
            void add(int fd, void *ptr, __uint32_t events);
            /**
             * @brief 修改监听句柄的模式
             *
             * @param fd 目标句柄
             * @param ptr 让相应的event_poll.data.ptr携带的数据
             * @param events EPOLLIN|EPOLOUT
             */
            void mod(int fd, void *ptr, __uint32_t events);
            /**
             * @brief 移去正在监听的句柄
             *
             * @param fd 目标句柄
             * @param ptr 设置event_poll.data.ptr携带的数据
             * @param events EPOLLIN|EPOLOUT
             */
            void del(int fd, void *ptr, __uint32_t events);
            /**
             * @brief 获取epoll事件
             *
             * @param millsecond 超时时间
             * @return int
             */
            int wait(int millsecond);

        protected:
            /**
             * @brief 控制 epoll，将EPOLL设为边缘触发EPOLLET模式
             * @param fd       句柄，在create函数时被赋值
             * @param ptr      辅助的数据, 可以后续在epoll_event中获取到
             * @param events   需要监听的事件
             *                 EPOLLIN  表示对应的文件描述符可以读
             *                 EPOLLOUT 表示对应的文件描述符可以写
             *                 EPOLLPRI 表示对应的文件描述符有紧急的数据可读
             *                 EPOLLERR 表示对应的文件描述符发生错误
             *                 EPOLLHUP 表示对应的文件描述符被挂断
             *                 EPOLLET  表示对应的文件描述符有事件发生
             * @param op       控制 epoll 文件描述符上的事件：注册、修改、删除
             *                 EPOLL_CTL_ADD：注册新的fd到epfd中
             *                 EPOLL_CTL_MOD：修改已经注册的fd的监听事件
             *                 EPOLL_CTL_DEL：从 epfd中删除一个fd；
             *
             */
            void ctrl(int fd, void *ptr, __uint32_t events, int op);

        protected:
            /**
             * @brief epoll句柄
             *
             */
            int m_epfd;
            /**
             * @brief 最大连接数，epoll_event最大数量
             *
             */
            int m_max_connections;
            /**
             * @brief 事件数组
             *
             */
            struct epoll_event *m_events;
            /**
             * @brief 是否为EPOLLET模式
             *
             */
            bool m_et;
        };
    }
};