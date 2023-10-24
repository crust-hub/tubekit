#include "util_time.h"

using namespace tubekit::ecsactor;

std::string timeutil::to_string(uint64_t time_tick)
{
    const auto milli = time_tick % 1000;
    time_t tick = (time_t)(time_tick / 1000);
    struct tm *tm = localtime(&tick);
    return strutil::format("%d-%02d-%02d %02d:%02d:%02d.%03d",
                           tm->tm_year + 1900,
                           tm->tm_mon + 1,
                           tm->tm_mday,
                           tm->tm_hour,
                           tm->tm_min,
                           tm->tm_sec,
                           milli);
}
