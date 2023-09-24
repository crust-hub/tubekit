#include "app/tick.h"
#include "tubekit-log/logger.h"
#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include "connection/connection.h"
#include "connection/stream_connection.h"

#include <iostream>
#include <ctime>

using namespace tubekit::app;
using namespace tubekit::utility;
using namespace tubekit::connection;

void tick::run()
{
    static time_t t = 0;
    time_t now = time(NULL);
    if (0 == t)
    {
        t = now;
    }
    if (now - t >= 1)
    {
        t = now;
        singleton<connection_mgr>::instance()->for_each([](connection::connection &conn)
                                                        {
                                                            if (connection_mgr::is_stream(&conn))
                                                            {
                                                                connection_mgr::convert_to_stream(&conn)->send("tick\n",5);
                                                                //conn.mark_close();
                                                            }
                                                            return; });
    }
}