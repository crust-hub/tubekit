#pragma once
#include <list>
#include <mutex>
#include "timer/timer.h"

/*
* PLAN: 需要后续优化，使用优先队列解决，待提高时间精度
*/

namespace tubekit
{
    namespace timer
    {
        class timer_manager final
        {
        public:
            timer_manager();
            ~timer_manager();
            /**
             * @brief 添加新的定时器
             *
             * @param repeated_times 重复次数，为-1则一直重复下去
             * @param interval 触发间隔
             * @param callback 回调函数
             * @return int64_t 返回新创建的定时器id
             */
            int64_t add(int32_t repeated_times, int64_t interval, const timer_callback callback);

            /**
             * @brief 删除定时器
             *
             * @param timer_id 定时器ID
             * @return true
             * @return false
             */
            bool remove(int64_t timer_id);

            /**
             * @brief 检测定时器，到期则触发执行
             *
             */
            void check_and_handle();

        private:
            std::list<timer *> m_list;
            std::mutex m_mutex;
        };
    }
}