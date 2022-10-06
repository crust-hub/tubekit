#include "thread/mutex.h"

using namespace tubekit::thread;

mutex::mutex()
{
    // create init attribute
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    // setting type of mutex
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&m_mutex, &attr);
    // destory attr
    pthread_mutexattr_destroy(&attr);
}

mutex::~mutex()
{
    // destory mutex
    pthread_mutex_destroy(&m_mutex);
}

int mutex::lock()
{
    return pthread_mutex_lock(&m_mutex);
}

int mutex::try_lock()
{
    return pthread_mutex_trylock(&m_mutex);
}

int mutex::unlock()
{
    return pthread_mutex_unlock(&m_mutex);
}
