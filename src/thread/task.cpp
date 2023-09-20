#include "thread/task.h"

#include <iostream>

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
    return m_data;
}

void task::set_data(void *data)
{
    m_data = data;
}

bool task::compare(task *other)
{
    if (this == other)
    {
        return true;
    }
    if (other && this->m_data == other->m_data)
    {
        return true;
    }
    return false;
}