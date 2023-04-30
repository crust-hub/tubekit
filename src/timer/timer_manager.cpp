#include "timer/timer_manager.h"

using namespace tubekit::timer;

timer_manager::timer_manager()
{
}

timer_manager::~timer_manager()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto iter = m_list.begin(); iter != m_list.end(); ++iter)
    {
        timer *timer_ptr = (*iter);
        delete timer_ptr;
    }
    m_list.clear();
}

int64_t timer_manager::add(int32_t repeated_times, int64_t interval, const timer_callback callback)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    timer *timer_ptr = new timer(repeated_times, interval, callback);
    if (timer_ptr == nullptr)
    {
        return -1;
    }
    m_list.push_back(timer_ptr);
    return timer_ptr->get_id();
}

bool timer_manager::remove(int64_t timer_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (timer_id < 0)
    {
        return false;
    }
    for (auto iter = m_list.begin(); iter != m_list.end(); ++iter)
    {
        if ((*iter)->get_id() == timer_id)
        {
            timer *timer_ptr = (*iter);
            delete timer_ptr;
            m_list.erase(iter);
            return true;
        }
    }
    return false;
}

void timer_manager::check_and_handle()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto iter = m_list.begin(); iter != m_list.end();)
    {
        if ((*iter)->is_expired())
        {
            (*iter)->run();
            int32_t times = (*iter)->get_repeated_times();
            if (times == 0)
            {
                timer *timer_ptr = *iter;
                iter = m_list.erase(iter);
                delete timer_ptr;
            }
        }
        else
        {
            ++iter;
        }
    }
}
