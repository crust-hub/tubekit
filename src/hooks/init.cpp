#include "hooks/init.h"
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

int init::run()
{
    task::task_type task_type = singleton<server::server>::instance()->get_task_type();

    try
    {
        switch (task_type)
        {
        case task::task_type::HTTP_TASK:
        {
            return http_app::on_init();
        }
        case task::task_type::STREAM_TASK:
        {
            return stream_app::on_init();
        }
        case task::task_type::WEBSOCKET_TASK:
        {
            return websocket_app::on_init();
        }
        default:
        {
            return -1;
        }
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(e.what());
        return -1;
    }
    return -1;
}