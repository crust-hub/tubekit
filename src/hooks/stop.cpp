#include "hooks/stop.h"
#include "server/server.h"
#include "utility/singleton.h"
#include "app/http_app.h"
#include "app/stream_app.h"
#include "app/websocket_app.h"
#include "task/task_type.h"

#include <iostream>
#include <stdexcept>
#include <tubekit-log/logger.h>

using namespace tubekit::app;
using namespace tubekit::hooks;
using namespace tubekit::server;
using namespace tubekit::utility;
using namespace tubekit::task;

void stop::run()
{
    task::task_type task_type = singleton<server::server>::instance()->get_task_type();
    try
    {
        switch (task_type)
        {
        case task::task_type::HTTP_TASK:
        {
            http_app::on_stop();
            break;
        }
        case task::task_type::STREAM_TASK:
        {
            stream_app::on_stop();
            break;
        }
        case task::task_type::WEBSOCKET_TASK:
        {
            websocket_app::on_stop();
            break;
        }
        default:
            break;
        }
    }
    catch (std::exception &e)
    {
        LOG_ERROR(e.what());
    }
}