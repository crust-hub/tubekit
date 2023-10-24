#pragma once
#include "util_string.h"

#include <chrono>
#include <sys/timeb.h>

namespace tubekit::ecsactor
{
    namespace timeutil
    {
        typedef uint64_t time;

        inline time add_seconds(time time_value, int second)
        {
            return time_value + (uint64_t)second * 1000;
        }

        inline time add_milliseconds(time time_value, int milliseconds)
        {
            return time_value + milliseconds;
        }

        std::string to_string(uint64_t time_tick);
    }
}
