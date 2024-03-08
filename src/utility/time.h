#include <cstdint>
#include <chrono>

namespace tubekit::utility::time
{
    class time
    {
    public:
        int64_t get_milliseconds();
        void update();
        std::chrono::system_clock::time_point m_time;
    };
}