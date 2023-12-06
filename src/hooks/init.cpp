#include "hooks/init.h"
#include "server/server.h"
#include "utility/singleton.h"
#include "app/http_app.h"
#include "app/stream_app.h"
#include "app/websocket_app.h"

#include <iostream>

using namespace tubekit::app;
using namespace tubekit::hooks;
using namespace tubekit::server;
using namespace tubekit::utility;

int init::run()
{
    server::server::TaskType task_type = singleton<server::server>::instance()->get_task_type();
    switch (task_type)
    {
    case server::server::TaskType::HTTP_TASK:
    {
        return http_app::on_init();
    }
    case server::server::TaskType::STREAM_TASK:
    {
        return stream_app::on_init();
    }
    case server::server::TaskType::WEBSOCKET_TASK:
    {
        return websocket_app::on_init();
    }
    default:
    {
        return -1;
    }
    }
    return -1;
}