#pragma once

#include <unordered_map>
#include <cstdint>

#include "thread/task.h"
#include "thread/worker.h"
#include "thread/thread.h"

namespace tubekit::thread
{
    class worker_pool
    {
    public:
        worker_pool();
        ~worker_pool();

        void create(size_t size);

        size_t get_size();

        void assign(task *m_task);

        void stop();

    private:
        std::unordered_map<uint32_t, worker *> worker_map;
    };
}