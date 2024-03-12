#include <tubekit-log/logger.h>
#include <thread>
#include <chrono>

#include "thread/worker.h"
#include "utility/singleton.h"
#include "thread/task.h"
#include "task/task_mgr.h"

using namespace tubekit::thread;
using namespace tubekit::log;
using namespace tubekit::utility;

worker::worker(task_destory *destory_ptr) : thread(),
                                            m_destory_ptr(destory_ptr),
                                            m_stoped(true)
{
}

worker::~worker()
{
}

void worker::cleanup(void *ptr)
{
    LOG_INFO("worker thread cleanup handler: %x", ptr);
    worker *worker_ptr = (worker *)ptr;
    worker_ptr->m_stoped = true;
}

void worker::run()
{
    LOG_ERROR("worker[%x]::run() begin", this);
    m_stoped = false;
    sigset_t mask;
    // sigfillset(sigset_t *set)调用该函数后，set指向的信号集中将包含linux支持的64种信号
    if (0 != sigfillset(&mask))
    {
        LOG_ERROR("worker thread sigfillset failed");
        return;
    }
    // SIG_BLOCK:结果集是当前集合参数集的并集；SIG_UNBLOCK:结果集是当前集合参数集的差集；SIG_SETMASK:结果集是由参数集指向的
    // 在多线程的程序里，希望只在主线程中处理信号，可以使用pthread_sigmask
    if (0 != pthread_sigmask(SIG_SETMASK, &mask, NULL))
    {
        LOG_ERROR("worker thread pthread_sigmask failed");
        return;
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

    while (true) // 线程将会一直运行，to process task
    {
        task *will_run_task = m_task_queue.pop();
        if (will_run_task == nullptr || stop_flag)
        {
            m_stoped = true;
            break;
        }
        // int rc = 0;
        int old_state = 0;
        // rc =
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
        /*
        设置本线程对Cancel信号的反应。
        state有两种值：pthread_CANCEL_ENABLE（缺省）和pthread_CANCEL_DISABLE
        分别表示收到信号后设为CANCLED状态和忽略CANCEL信号继续运行；
        old_state如果不为NULL则存入原来的Cancel状态以便恢复。
        在执行任务期间不允许被cancel
        */

        // 执行任务
        will_run_task->run();

        // destory task
        m_destory_ptr->execute(will_run_task);

        will_run_task = nullptr;

        // 允许接收cancel信号后被设置为CANCLED状态 然后运行到取消点停止
        // rc =
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old_state);

        pthread_testcancel(); // 设置取消点 如果线程收到了cancel则运行到取消点才可以被取消
    }

    pthread_cleanup_pop(1);
    m_stoped = true;
}

void worker::push(task *task_ptr)
{
    if (stop_flag)
    {
        if (task_ptr)
        {
            m_destory_ptr->execute(task_ptr);
        }
        LOG_ERROR("worker thread already stoped, cannot add task to queue");
        return;
    }

    bool b_res = m_task_queue.push(task_ptr);
    if (false == b_res)
    {
        // destory task
        if (task_ptr)
        {
            m_destory_ptr->execute(task_ptr);
        }
    }
}

void worker::stop()
{
    push(nullptr);
    stop_flag = true;
    // wait task_queue empty
    LOG_ERROR("sleep_for worker[%x] m_stoped", this);
    while (false == m_stoped)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    LOG_ERROR("okay sleep_for worker[%x] m_stoped", this);
}