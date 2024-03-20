// g++ udp_client.cpp udp_component.cpp -o udp_client.exe --std=c++11
#include <iostream>
#include "udp_component.h"
using namespace std;
using namespace tubekit::ipc;

int main(int argc, char **argv)
{
    udp_component udp;
    uint64_t pingpong_count = 0;

    udp.tick_callback = [&udp](bool &to_stop)
    {
        // std::cout<<"on_tick"<<std::endl;
    };

    udp.message_callback = [&udp, &pingpong_count](const char *buffer, ssize_t len, sockaddr_in &addr)
    {
        pingpong_count++;
        std::cout << std::string(buffer, len) << " pingpong_count " << pingpong_count << std::endl;
        // pingpong
        udp.udp_component_client("", 0,
                                 buffer,
                                 len,
                                 (sockaddr *)&addr,
                                 sizeof(addr));
    };

    udp.close_callback = [&udp]()
    {
        // close callback
    };

    udp.udp_component_client("172.29.94.203", 20025, "pingpong", 8);
    return 0;
}
