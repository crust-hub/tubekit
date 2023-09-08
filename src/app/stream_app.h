#pragma once
#include "connection/stream_connection.h"

namespace tubekit
{
    namespace app
    {
        class stream_app
        {
        public:
            static void process_connection(tubekit::connection::stream_connection &m_stream_connection);
        };
    }
}
