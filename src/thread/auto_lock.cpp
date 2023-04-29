#include "thread/auto_lock.h"

using namespace tubekit::thread;

auto_lock::auto_lock(mutex &mutex) : m_mutex(mutex)
{
    m_mutex.lock();
}

auto_lock::~auto_lock()
{
    m_mutex.unlock();
}
