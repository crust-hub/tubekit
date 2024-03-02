// g++ udp_server.cpp udp_component.cpp -o udp_server.exe --std=c++11
#include <iostream>
#include "udp_component.h"
using namespace std;
using namespace tubekit::ipc;

int main(int argc, char **argv)
{
    bool stop_flag = false;
    udp_component_server(
        "192.168.20.138", "20023", 10, stop_flag,
        []()
        {
            // tick
            udp_component_client("192.168.20.138", 20024, "hello", 6);
        },
        [&stop_flag](const char *buffer, ssize_t len, sockaddr_in &addr)
        {
            std::cout << std::string(buffer, len) << std::endl;
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
