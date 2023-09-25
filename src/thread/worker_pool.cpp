#include "thread/worker_pool.h"

#include <tubekit-log/logger.h>

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

void worker_pool::create(size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        worker *new_worker = new worker();
        LOG_DEBUG("create worker thread %x", new_worker);
        worker_map[i] = new_worker;
        new_worker->start();
    }
}

void worker_pool::assign(task *m_task)
{
    std::uintptr_t uintptr = reinterpret_cast<std::uintptr_t>(m_task->get_data());
    std::uint32_t hash = uintptr;

    uint32_t addr2idx = hash;
    addr2idx = addr2idx >> 16;
    addr2idx = (addr2idx + 5) % worker_map.size();

    // LOG_ERROR("addr2idx=%d", addr2idx);

    worker_map[addr2idx]->push(m_task);
}

void worker_pool::stop()
{
    for (auto m_pair : worker_map)
    {
        m_pair.second->stop();
    }
}
