#pragma once
#include "util_string.h"

#include <chrono>
#include <sys/timeb.h>

namespace tubekit::ecsactor
{
    namespace timeutil
    {
        /**
         * @brief 时间戳 毫秒
         *
         */
        typedef uint64_t time;

        /**
         * @brief 在time_value基础上加second秒
         *
         * @param time_value
         * @param second
         * @return time
         */
        inline time add_seconds(time time_value, int second)
        {
            return time_value + (uint64_t)second * 1000;
        }

        /**
         * @brief 在time_value基础上加milliseconds毫秒
         *
         * @param time_value
         * @param milliseconds
         * @return time
         */
        inline time add_milliseconds(time time_value, int milliseconds)
        {
            return time_value + milliseconds;
        }

        /**
         * @brief 获取此刻时间戳 毫秒
         *
         * @return time
         */
        inline time get_milliseconds()
        {
            auto time_value = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            return time_value.time_since_epoch().count();
        }

        /**
         * @brief 事件戳转字符串时间表示
         *
         * @param time_tick
         * @return std::string
         */
        std::string to_string(uint64_t time_tick);
    }
}
