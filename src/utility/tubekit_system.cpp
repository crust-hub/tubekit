#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <cstring>

#include "tubekit_system.h"
#include "log/logger.h"

using namespace tubekit::utility;

void tubekit_system::init()
{
    core_dump();
    m_root_path = get_root_path();
    // check log dir or create it
    const string log_dir_path = m_root_path + "/log";
    DIR *dp = opendir(log_dir_path.c_str());
    if (dp == nullptr)
        mkdir(log_dir_path.c_str(), 0755);
    else
        closedir(dp);
    // init logger
    tubekit::log::logger::instance()->open(m_root_path + "/log/tubekit.log");
    // init inifile

    // init workflow
}

void tubekit_system::core_dump()
{
    // core dump info
    struct rlimit x;
    int ret = getrlimit(RLIMIT_CORE, &x);
    x.rlim_cur = x.rlim_max;
    ret = setrlimit(RLIMIT_CORE, &x);
    ret = getrlimit(RLIMIT_DATA, &x);
    x.rlim_cur = 768000000;
    ret = setrlimit(RLIMIT_DATA, &x);
}

string tubekit_system::get_root_path()
{
    if (m_root_path != "")
    {
        return m_root_path;
    }
    char path[1024];
    memset(path, 0, 1024);                            // setting all memory byte to zero
    int cnt = readlink("/proc/self/exe", path, 1024); // system operator
    if (cnt < 0 || cnt >= 1024)
    {
        return "";
    }
    // find last charactor /,change to \0
    for (int i = cnt; i >= 0; --i)
    {
        if (path[i] == '/')
        {
            path[i] = '\0';
            break;
        }
    }
    return string(path);
}