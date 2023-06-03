#include "pool.h"

using pool = tubekit::sql::pool;
using connection = tubekit::sql::connection;

pool::pool()
{
}

pool::~pool()
{
}

void pool::init(int size,
                const std::string &ip,
                const std::string &user,
                const std::string &password,
                const std::string &db)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (int i = 0; i < size; i++)
    {
        std::shared_ptr<connection> conn = std::make_shared<connection>();
        bool result = conn->connect(ip, user, password, db);
        if (result)
        {
            m_list.push_back(conn);
        }
    }
    cv.notify_all();
}

std::shared_ptr<connection> pool::get()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    cv.wait(lock,
            [&]
            {
                return !m_list.empty();
            });
    auto ptr = *m_list.begin();
    m_list.pop_front();
    return ptr;
}

void pool::back(std::shared_ptr<connection> ptr)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_list.push_back(ptr);
    cv.notify_all();
}

int pool::get_size()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_list.size();
}