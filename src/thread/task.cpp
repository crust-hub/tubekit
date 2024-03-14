#include "thread/task.h"

#include <iostream>

using namespace tubekit::thread;

task::task(uint64_t gid) : m_gid(gid)
{
}

task::~task()
{
}

uint64_t task::get_gid()
{
    return m_gid;
}

void task::set_gid(uint64_t gid)
{
    m_gid = gid;
}

bool task::compare(task *other)
{
    if (this == other)
    {
        return true;
    }
    if (other && this->m_gid == other->m_gid)
    {
        return true;
    }
    return false;
}