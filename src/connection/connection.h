#pragma once
#include "socket/socket.h"

namespace tubekit
{
    namespace connection
    {
        class connection
        {
        public:
            connection(tubekit::socket::socket *socket_ptr);
            virtual ~connection();
            virtual void close_before();

        public:
            virtual void on_mark_close() = 0;

        public:
            /**
             * @brief connection will be closed
             *
             */
            void mark_close();
            bool is_close();
            tubekit::socket::socket *get_socket_ptr();
            void set_socket_ptr(tubekit::socket::socket *socket_ptr);
            /**
             * @brief be executed when allocating from object_pool
             *
             */
            virtual void reuse();

            void set_gid(uint64_t gid);
            uint64_t get_gid();

        private:
            bool close_flag{false};
            uint64_t gid{0};

        protected:
            tubekit::socket::socket *socket_ptr{nullptr};
        };
    }
}