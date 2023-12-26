#pragma once
#include <string>
#include "connection/stream_connection.h"

namespace tubekit
{
    namespace app
    {
        class stream_app
        {
        public:
            static void process_connection(tubekit::connection::stream_connection &m_stream_connection);
            static void on_close_connection(tubekit::connection::stream_connection &m_stream_connection);
            static void on_new_connection(tubekit::connection::stream_connection &m_stream_connection);

            static int on_init();
            static void on_stop();
            static void on_tick();
        };
    }
}
