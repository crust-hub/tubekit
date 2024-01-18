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
        /**
         * @brief Construct a new global object
         *
         * @param app_type 应用类型
         * @param app_id 应用id
         */
        global(uint32_t app_type, uint32_t app_id);

        /**
         * @brief 更新时间
         *
         */
        void update_time();

        /**
         * @brief 从标识中获取appid
         *
         * @param sn
         * @return uint32_t
         */
        static uint32_t get_app_id_from_sn(uint64_t sn);

        /**
         * @brief 生成唯一标识
         *
         * @return uint64_t
         */
        uint64_t generate_sn();

        /**
         * @brief 生成uuid
         *
         * @return std::string
         */
        static std::string generate_uuid();

        /**
         * @brief 获取apptype
         *
         * @return uint32_t
         */
        uint32_t get_cur_app_type() const;

        /**
         * @brief 获取appid
         *
         * @return uint32_t
         */
        uint32_t get_cur_app_id() const;

        /**
         * @brief 上次update_time的时间戳 毫秒
         *
         */
        timeutil::time time_tick;

        /**
         * @brief 停止标识
         *
         */
        bool is_stop{false};

    private:
        std::mutex m_mutex;
        uint16_t m_sn_ticket{1};
        uint32_t m_app_type;
        uint32_t m_app_id;
    };
};