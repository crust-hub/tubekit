#include "thread/tubekit_thread.h"
#include "thread/auto_lock.h"

using namespace tubekit::thread;

tubekit_thread::tubekit_thread() : m_tid(0), m_task(nullptr)
{
}

tubekit_thread::~tubekit_thread()
{
}

void tubekit_thread::start()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // pthread_attr_setscope 参数
    // PTHREAD_SCOPE_PROCESS 该线程与同一进程中的所有其他线程争用资源，这些线程也是使用PTHREAD_SCOPE_PROCESS竞争范围创建的
    // PTHREAD_SCOPE_SYSTEM 线程与系统上所有进程中位于同一调度分配域(一组一个或多个处理器)中的所有其他线程竞争资源
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    // pthread_attr_setdetachstate，pthread_attr_getdetachstate-在线程属性对象中设置/获取分离状态属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // PTHREAD_CREATE_DETACHED | PTHREAD_CREATE_JOINABLE

    pthread_create(&m_tid, &attr, thread_func, (void *)this);
    pthread_attr_destroy(&attr);
}

void tubekit_thread::stop()
{
    pthread_exit(PTHREAD_CANCELED);
}

void *tubekit_thread::thread_func(void *ptr)
{
    tubekit_thread *m_thread = (tubekit_thread *)ptr;
    m_thread->run();
    return ptr;
}

void tubekit_thread::set_task(task *task)
{
    m_mutex.lock();
    m_task = task;
    m_cond.signal();
    m_mutex.unlock();
}

task *tubekit_thread::get_task()
{
    auto_lock lock(m_mutex);
    return m_task;
}
