#include <iostream>
#include <string>
#include "utility/singleton_template.h"
#include "utility/tubekit_system.h"
#include "inifile/inifile.h"
#include "log/logger.h"
using namespace std;
using namespace tubekit::inifile;
using namespace tubekit::utility;
using namespace tubekit::log;

int main(int argc, char **argv)
{
    auto *ptr = singleton_template<tubekit_system>::instance();
    ptr->init(); // system init

    inifile *ini = singleton_template<inifile>::instance();
    const string &ip = (*ini)["server"]["ip"];
    int port = (*ini)["server"]["port"];
    int threads = (*ini)["server"]["threads"];
    int max_conn = (*ini)["server"]["max_conn"];
    int wait_time = (*ini)["server"]["wait_time"];

    logger::instance()->debug("server config", 1, "ip: %s port: %d threads: %d max_conn: %d wait_time: %d ", ip.c_str(), port, threads, max_conn, wait_time);
    return 0;
}