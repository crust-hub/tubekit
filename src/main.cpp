#include <iostream>
#include <string>

#include "utility/singleton.h"
#include "system/system.h"

using namespace tubekit::utility;

int main(int argc, char **argv)
{
    singleton<tubekit::system::system>::instance()->init(); // system init
    return 0;
}