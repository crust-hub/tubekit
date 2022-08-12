#include <iostream>
#include <string>
#include "utility/singleton_template.h"
#include "utility/tubekit_system.h"
using namespace std;

int main(int argc, char **argv)
{
    auto *ptr = tubekit::utility::singleton_template<tubekit::utility::tubekit_system>::instance();
    cout << ptr->get_root_path() << endl;
    cout << R"(
        todo_list:
            tubekit_system.init();
    )" << endl;
    return 0;
}