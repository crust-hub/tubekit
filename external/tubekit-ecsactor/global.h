#pragma once

#include "util_time.h"
#include "singleton.h"
#include <mutex>
#include <string>

namespace tubekit::ecsactor
{
    class global : public singleton<global>
    {
    public:
        global(uint32_t app_type, uint32_t app_id);
        void update_time();

        static uint32_t get_app_id_from_sn(uint64_t sn);
        uint64_t generate_sn();
        static std::string generate_uuid();

        uint32_t get_cur_app_type() const;
        uint32_t get_cur_app_id() const;

        timeutil::time time_tick;
        bool is_stop{false};

    private:
        std::mutex m_mutex;
        uint16_t m_sn_ticket{1};
        uint32_t m_app_type;
        uint32_t m_app_id;
    };
};