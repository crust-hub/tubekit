#pragma once
#include <stddef.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string>
#include <cstring>
#include <unordered_map>

namespace tubekit
{
    namespace socket
    {
        class event_poller
        {
            friend class socket_handler;

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
             * @brief 向epoll句柄添加新的epoll_event
             *
             * @param fd epoll句柄
             * @param ptr 创建的epoll_event携带的数据指针
             * @param events EPOLLIN: 表示对应的文件描述符可以读
             *               EPOLLOUT: 表示对应的文件描述符可以写
             *               EPOLLPRI: 表示对应的文件描述符有紧急的数可读
             *               EPOLLERR: 表示对应的文件描述符发生错误
             *               EPOLLHUP: 表示对应的文件描述符被挂断
             *               EPOLLET: ET的epoll工作模式
             *               EPOLLLT: 表示默认epoll工作模式
             *               EPOLLONETSHOT: 表示
             *               epoll模式中事件可能被触发多次，比如socket接收到数据交给一个线程处理数据，
             *               在数据没有处理完之前又有新数据达到触发了事件，另一个线程被激活获得该socket，
             *               从而产生多个线程操作同一socket，即使在ET模式下也有可能出现这种情况。
             *               采用EPOLLONETSHOT事件的文件描述符上的注册事件只触发一次，要想重新注册事件则需要调用epoll_ctl重置文件描述符上的事件，
             *               这样前面的socket就不会出现竞态。
             */
            int add(int fd, void *ptr, uint32_t events);
            /**
             * @brief epoll句柄更新已存在的epoll_event
             *
             * @param fd epoll句柄
             * @param ptr 创建的epoll_event携带的数据指针
             * @param events EPOLLIN: 表示对应的文件描述符可以读
             *               EPOLLOUT: 表示对应的文件描述符可以写
             *               EPOLLPRI: 表示对应的文件描述符有紧急的数可读
             *               EPOLLERR: 表示对应的文件描述符发生错误
             *               EPOLLHUP: 表示对应的文件描述符被挂断
             *               EPOLLET: ET的epoll工作模式
             *               EPOLLLT: 表示默认epoll工作模式
             *               EPOLLONETSHOT: 表示
             *               epoll模式中事件可能被触发多次，比如socket接收到数据交给一个线程处理数据，
             *               在数据没有处理完之前又有新数据达到触发了事件，另一个线程被激活获得该socket，
             *               从而产生多个线程操作同一socket，即使在ET模式下也有可能出现这种情况。
             *               采用EPOLLONETSHOT事件的文件描述符上的注册事件只触发一次，要想重新注册事件则需要调用epoll_ctl重置文件描述符上的事件，
             *               这样前面的socket就不会出现竞态。
             */
            int mod(int fd, void *ptr, uint32_t events);
            /**
             * @brief 从epoll句柄删除epoll_event
             *
             * @param fd epoll句柄
             * @param ptr 创建的epoll_event携带的数据指针
             * @param events EPOLLIN: 表示对应的文件描述符可以读
             *               EPOLLOUT: 表示对应的文件描述符可以写
             *               EPOLLPRI: 表示对应的文件描述符有紧急的数可读
             *               EPOLLERR: 表示对应的文件描述符发生错误
             *               EPOLLHUP: 表示对应的文件描述符被挂断
             *               EPOLLET: ET的epoll工作模式
             *               EPOLLLT: 表示默认epoll工作模式
             *               EPOLLONETSHOT: 表示
             *               epoll模式中事件可能被触发多次，比如socket接收到数据交给一个线程处理数据，
             *               在数据没有处理完之前又有新数据达到触发了事件，另一个线程被激活获得该socket，
             *               从而产生多个线程操作同一socket，即使在ET模式下也有可能出现这种情况。
             *               采用EPOLLONETSHOT事件的文件描述符上的注册事件只触发一次，要想重新注册事件则需要调用epoll_ctl重置文件描述符上的事件，
             *               这样前面的socket就不会出现竞态。
             */
            int del(int fd, void *ptr, uint32_t events);
            /**
             * @brief 获取epoll事件
             *
             * @param millsecond 超时时间
             * @return int
             */
            int wait(int millsecond);

            uint32_t get_events_by_fd(int fd);

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
            int ctrl(int fd, void *ptr, uint32_t events, int op);

        protected:
            /**
             * @brief epoll句柄
             *
             */
            int m_epfd{0};
            /**
             * @brief 最大连接数，epoll_event最大数量
             *
             */
            int m_max_connections{0};
            /**
             * @brief 事件数组
             *
             */
            struct epoll_event *m_events{nullptr};
            /**
             * @brief 是否为EPOLLET模式
             *
             */
            bool m_et{false};
            /**
             * @brief event what store fd is listenning
             *
             */
            std::unordered_map<int, uint32_t> m_map_fd_envets{};
        };
    }
};