#include "global.h"
#include "util_uuid.h"

using tubekit::ecsactor::global;

global::global(uint32_t app_type, uint32_t app_id)
{
    m_app_type = app_type;
    m_app_id = app_id;
    update_time();
}

uint32_t global::get_app_id_from_sn(uint64_t sn)
{
    sn = sn << 38;
    sn = sn >> 38; // 76 - 64 = 12
    sn = sn >> 16;
    return static_cast<uint32_t>(sn);
}

uint64_t global::generate_sn()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    // (38[time],10bit[app_id],16[ticket])
    const uint64_t ret = ((time_tick >> 10) << 26) + (m_app_id << 16) + m_sn_ticket;
    m_sn_ticket += 1;
    return ret;
}

uint32_t global::get_cur_app_type() const
{
    return m_app_type;
}

uint32_t global::get_cur_app_id() const
{
    return m_app_id;
}

void global::update_time()
{
    time_tick = timeutil::get_milliseconds();
}

std::string global::generate_uuid()
{
    return ecsactor::generate_uuid();
}