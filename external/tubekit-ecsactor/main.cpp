#include <iostream>
#include <vector>
#include "util_time.h"
#include "util_uuid.h"
#include "global.h"
using namespace std;
using namespace tubekit::ecsactor;

int main(int argc, char **argv)
{
    timeutil::time point = 1000;                          // 1000 milliseconds
    cout << timeutil::add_milliseconds(point, 5) << endl; // 1005
    cout << timeutil::add_seconds(point, 3) << endl;      // 4000
    cout << timeutil::to_string(timeutil::add_milliseconds(point, 5)) << endl;
    // 1970-01-01 08:00:01.005
    std::string uuid = generate_uuid();
    cout << uuid << endl;

    global::instance(1, 2);
    uint64_t sn = global::get_instance()->generate_sn();
    cout << global::get_instance()->get_app_id_from_sn(sn) << endl;
    cout << global::get_instance()->generate_uuid() << endl;

    return 0;
}