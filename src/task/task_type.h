#pragma once

namespace tubekit::task
{
    enum task_type
    {
        HTTP_TASK = 0,
        STREAM_TASK,
        WEBSOCKET_TASK,
        NONE
    };
}