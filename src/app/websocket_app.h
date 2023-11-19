#pragma once
#include <string>
#include "connection/websocket_connection.h"

namespace tubekit
{
    namespace app
    {
        class websocket_app
        {
        public:
            static void process_connection(tubekit::connection::websocket_connection &m_websocket_connection);
            static void on_close_connection(tubekit::connection::websocket_connection &m_websocket_connection);
            static void on_new_connection(tubekit::connection::websocket_connection &m_websocket_connection);
            static bool send_packet(tubekit::connection::websocket_connection &m_websocket_connection, const char *data, size_t data_len, bool use_safe);
        };
    }
}
