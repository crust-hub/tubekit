#include "thread/worker_pool.h"

#include <tubekit-log/logger.h>
#include <limits>

using namespace tubekit::thread;
using namespace tubekit::log;

worker_pool::worker_pool()
{
}

worker_pool::~worker_pool()
{
}

size_t worker_pool::get_size()
{
    return worker_map.size();
}

void worker_pool::create(size_t size, task_destory *destory_ptr)
{
    for (size_t i = 0; i < size; i++)
    {
        worker *new_worker = new worker(destory_ptr);
        LOG_DEBUG("create worker thread %x", new_worker);
        worker_map[i] = new_worker;
        new_worker->start();
    }
}

void worker_pool::assign(task *m_task, uint64_t hash_key)
{
    hash_key = hash_key % worker_map.size();

    worker_map[hash_key]->push(m_task);
}

void worker_pool::stop()
{
    for (auto m_pair : worker_map)
    {
        m_pair.second->stop();
    }
}
