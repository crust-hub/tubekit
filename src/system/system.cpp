#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <cstring>
#include <iostream>
#include <filesystem>
#include <fcntl.h>
#include <tubekit-inifile/inifile.h>
#include <tubekit-log/logger.h>

#include "system/system.h"
#include "utility/singleton.h"
#include "engine/workflow.h"
#include "server/server.h"

using namespace tubekit::utility;
using namespace tubekit::system;
using namespace tubekit::inifile;
using namespace tubekit::log;
using namespace tubekit::server;
using namespace std;

void system::init()
{
    // init inifile
    inifile::inifile *ini = singleton<inifile::inifile>::instance();
    ini->load(get_root_path() + "/config/main.ini");
    const string &ip = (*ini)["server"]["ip"];
    const int port = (*ini)["server"]["port"];
    const int threads = (*ini)["server"]["threads"];
    const int max_conn = (*ini)["server"]["max_conn"];
    const int wait_time = (*ini)["server"]["wait_time"];
    const string task_type = (*ini)["server"]["task_type"];
    const int daemon = (*ini)["server"]["daemon"];
    if (daemon)
    {
        create_daemon();
    }
    core_dump();

    signal_conf(); // system signal process hook bind

    m_root_path = get_root_path();
    // check log dir or create it
    const string log_dir_path = m_root_path + "/log";
    DIR *dp = opendir(log_dir_path.c_str());
    if (dp == nullptr)
        mkdir(log_dir_path.c_str(), 0755); // create dir
    else
        closedir(dp);

    // init logger
    logger::instance().open(m_root_path + "/log/tubekit.log");

    //  init workflow
    // engine::workflow *work = singleton<engine::workflow>::instance();
    // work->load(get_root_path() + "/config/workflow.xml");

    // server start
    // init server
    auto m_server = singleton<server::server>::instance();
    m_server->config(ip, port, threads, max_conn, wait_time, task_type, daemon);
    m_server->start(); // server running with dead loop
}

void system::core_dump()
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

string system::get_root_path()
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

void system::signal_conf()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    // signal(SIGINT, );
    signal(SIGTERM, &system::signal_term); // to call server::to_stop
    signal(SIGSTOP, &system::signal_term);
    signal(SIGKILL, &system::signal_term);
}

void system::create_daemon()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        std::cerr << "server::create_daemon() fork error" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (pid > 0) // father
    {
        exit(EXIT_SUCCESS);
    }
    // child
    setsid();
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0)
    {
        std::cerr << "server::create_daemon() open" << std::endl;
        exit(EXIT_FAILURE);
    }
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (fd > 2)
    {
        close(fd);
    }
}

void system::signal_term(int sig)
{
    singleton<server::server>::instance()->to_stop();
}