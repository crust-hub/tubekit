#include "thread/condition.h"
using namespace tubekit::thread;

condition::condition()
{
    pthread_cond_init(&m_cond, nullptr); // init condition
}

condition::~condition()
{
    pthread_cond_destroy(&m_cond);
}

int condition::wait(mutex *m_mutex)
{
    return pthread_cond_wait(&m_cond, &(m_mutex->m_mutex));
}

int condition::signal()
{
    return pthread_cond_signal(&m_cond);
}

int condition::broadcast()
{
    return pthread_cond_broadcast(&m_cond);
}
