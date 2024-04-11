#pragma once
#include "connection/http_connection.h"

// thread not safe : The function will be called by multiple threads simultaneously.
// thread safe : The function can only be called by the main thread or can be used in any thread.

namespace tubekit
{
    namespace app
    {
        class http_app
        {
        public:
            /**
             * @brief thread not safe
             *
             * @param m_http_connection
             */
            static void process_connection(tubekit::connection::http_connection &m_http_connection);

            /**
             * @brief thread not safe
             *
             * @param m_http_connection
             * @return int
             */
            static int on_body(tubekit::connection::http_connection &m_http_connection);

            /**
             * @brief thread safe
             *
             * @return int
             */
            static int on_init();

            /**
             * @brief thread safe
             *
             */
            static void on_stop();

            /**
             * @brief thread safe
             *
             */
            static void on_tick();
        };
    }
}