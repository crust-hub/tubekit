#include "thread/task.h"

#include <iostream>
#include "thread/auto_lock.h"

using namespace tubekit::thread;

task::task() : m_data(nullptr)
{
}

task::task(void *data) : m_data(data)
{
}

task::~task()
{
}

void *task::get_data()
{
    auto_lock lock(m_mutex);
    return m_data;
}

void task::set_data(void *data)
{
    auto_lock lock(m_mutex);
    m_data = data;
}
