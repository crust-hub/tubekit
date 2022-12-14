#include "thread/work_thread.h"
#include "utility/singleton_template.h"
#include "log/logger.h"
#include "thread/thread_pool.h"
#include "thread/task.h"

using namespace tubekit::thread;
using namespace tubekit::log;
using namespace tubekit::utility;

work_thread::work_thread() : thread()
{
}

work_thread::~work_thread()
{
}

void work_thread::cleanup(void *ptr)
{
    singleton_template<logger>::instance()->info("work_thread", 1, "worker thread cleanup handler: %x", ptr);
}

void work_thread::run()
{
    sigset_t mask;
    // sigfillset(sigset_t *set)调用该函数后，set指向的信号集中将包含linux支持的64种信号
    if (0 != sigfillset(&mask))
    {
        singleton_template<logger>::instance()->error("work_thread", 1, "worker thread sigfillset failed");
    }
    // SIG_BLOCK:结果集是当前集合参数集的并集；SIG_UNBLOCK:结果集是当前集合参数集的差集；SIG_SETMASK:结果集是由参数集指向的
    // 在多线程的程序里，希望只在主线程中处理信号，可以使用pthread_sigmask
    if (0 != pthread_sigmask(SIG_SETMASK, &mask, NULL))
    {
        singleton_template<logger>::instance()->error("work_thread", 1, "worker thread pthread_sigmask failed");
    }
    /*
    在POSIX线程API中提供了一个pthread_cleanup_push()/pthread_cleanup_pop()函数对用于自动释放资源
    --从pthread_cleanup_push()的调用点到pthread_cleanup_pop()之间的程序段中的终止动作
    （包括调用 pthread_exit()和取消点终止）都将执行pthread_cleanup_push()所指定的清理函数
    pthread_cleanup_push()/pthread_cleanup_pop()采用先入后出的栈结构管理，
    void routine(void *arg)函数在调用pthread_cleanup_push()时压入清理函数栈，
    多次对pthread_cleanup_push()的调用将在清理函数栈中形成一个函数链，
    在执行该函数链时按照压栈的相反顺序弹出。execute参数表示执行到pthread_cleanup_pop()时
    是否在弹出清理函数的同时执行该函数，为0表示不执行，非0为执行；这个参数并不影响异常终止时清理函数的执行
    如果pthread_cleanup_push与pthread_cleanup_pop之间有异常 不是正常返回等，则会触发cleanup自定义函数
    */
    pthread_cleanup_push(cleanup, this);

    while (true) //线程将会一直运行，to process task
    {
        m_mutex.lock();
        while (m_task == nullptr)
        {
            m_cond.wait(&m_mutex); //等待分发任务
        }
        m_mutex.unlock();

        int rc = 0;
        int old_state = 0;
        rc = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        /*
        设置本线程对Cancel信号的反应。
        state有两种值：pthread_CANCEL_ENABLE（缺省）和pthread_CANCEL_DISABLE
        分别表示收到信号后设为CANCLED状态和忽略CANCEL信号继续运行；
        old_state如果不为NULL则存入原来的Cancel状态以便恢复。
        在执行任务期间不允许被cancel
        */

        //执行任务
        m_task->run();
        m_task->destroy();
        m_task = nullptr;

        //将线程移到线程池空闲列表
        singleton_template<thread_pool<work_thread, task>>::instance()->move_to_idle_list(this);

        //允许接收cancel信号后被设置为CANCLED状态 然后运行到取消点停止
        rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);

        pthread_testcancel(); //设置取消点 如果线程收到了cancel则运行到取消点才可以被取消
    }

    pthread_cleanup_pop(1);
}