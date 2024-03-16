#pragma once
#include <string>
#include "connection/stream_connection.h"
#include <set>
#include "thread/mutex.h"

// thread not safe : The function will be called by multiple threads simultaneously.
// thread safe : The function can only be called by the main thread or can be used in any thread.

namespace tubekit
{
    namespace app
    {
        class stream_app
        {
        public:
            /**
             * @brief thread not safe
             *
             * @param m_stream_connection
             */
            static void process_connection(tubekit::connection::stream_connection &m_stream_connection);

            /**
             * @brief thread not safe
             *
             * @param m_stream_connection
             */
            static void on_close_connection(tubekit::connection::stream_connection &m_stream_connection);

            /**
             * @brief thread not safe
             *
             * @param m_stream_connection
             */
            static void on_new_connection(tubekit::connection::stream_connection &m_stream_connection);

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

            static std::set<uint64_t> global_player;
            static tubekit::thread::mutex global_player_mutex;
        };
    }
}
