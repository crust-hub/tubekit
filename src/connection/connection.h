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

        private:
            bool close_flag;

        protected:
            tubekit::socket::socket *socket_ptr;
        };
    }
}