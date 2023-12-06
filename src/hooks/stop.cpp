#include "hooks/stop.h"
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

void stop::run()
{
    server::server::TaskType task_type = singleton<server::server>::instance()->get_task_type();
    switch (task_type)
    {
    case server::server::TaskType::HTTP_TASK:
    {
        http_app::on_stop();
        break;
    }
    case server::server::TaskType::STREAM_TASK:
    {
        stream_app::on_stop();
        break;
    }
    case server::server::TaskType::WEBSOCKET_TASK:
    {
        websocket_app::on_stop();
        break;
    }
    default:
        break;
    }
}