#pragma once
#include <string>
#include <cstdint>
#include "connection/websocket_connection.h"

// thread not safe : The function will be called by multiple threads simultaneously.
// thread safe : The function can only be called by the main thread or can be used in any thread.

namespace tubekit
{
    namespace app
    {
        class websocket_app
        {
        public:
            /**
             * @brief thread not safe
             *
             * @param m_websocket_connection
             */
            static void process_connection(tubekit::connection::websocket_connection &m_websocket_connection);

            /**
             * @brief thread not safe
             *
             * @param m_websocket_connection
             */
            static void on_close_connection(tubekit::connection::websocket_connection &m_websocket_connection);

            /**
             * @brief thread not safe
             *
             * @param m_websocket_connection
             */
            static void on_new_connection(tubekit::connection::websocket_connection &m_websocket_connection);

            /**
             * @brief thread safe
             *
             * @param m_websocket_connection
             * @param data
             * @param data_len
             * @param gid
             * @return true
             * @return false
             */
            static bool send_packet(tubekit::connection::websocket_connection *m_websocket_connection, const char *data, size_t data_len, uint64_t gid = 0);

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
