#include <iostream>
#include <string>

#include "utility/singleton_template.h"
#include "system/system.h"
#include "inifile/inifile.h"
#include "log/logger.h"

using namespace std;
using namespace tubekit::inifile;
using namespace tubekit::utility;
using namespace tubekit::log;

int main(int argc, char **argv)
{
    singleton_template<tubekit::system::system>::instance()->init(); // system init
    return 0;
}