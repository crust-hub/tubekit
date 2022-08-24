#include <iostream>
#include <string>
#include "utility/singleton_template.h"
#include "utility/tubekit_system.h"
#include "log/logger.h"
using namespace std;

int main(int argc, char **argv)
{
    auto *ptr = tubekit::utility::singleton_template<tubekit::utility::tubekit_system>::instance();
    ptr->init();                                                                            // system init
    tubekit::log::logger::instance()->debug("sdcs", 1, "%s", ptr->get_root_path().c_str()); // log testing
    return 0;
}