// g++ udp_client.cpp udp_component.cpp -o udp_client.exe --std=c++11
#include <iostream>
#include "udp_component.h"
using namespace std;
using namespace tubekit::ipc;

int main(int argc, char **argv)
{
    bool stop_flag = false;
    udp_component_server(
        "192.168.20.138", "20024", 10, stop_flag,
        []()
        {
            // tick
        },
        [&stop_flag](const char *buffer, ssize_t len, sockaddr_in &addr)
        {
            udp_component_client(udp_component_get_ip(addr),
                                 20023,
                                 buffer,
                                 len);
            if (std::string(buffer, len) == "stop")
            {
                stop_flag = true;
            }
        },
        []()
        {
            // close callback
        });
    return 0;
}
