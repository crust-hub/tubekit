#include "time.h"

using namespace tubekit::utility::time;

int64_t time::get_milliseconds()
{
    return std::chrono::time_point_cast<std::chrono::milliseconds>(m_time).time_since_epoch().count();
}

void time::update()
{
    m_time = std::chrono::system_clock::now();
}