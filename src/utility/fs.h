#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <cstring>

namespace tubekit
{
    namespace fs
    {
        enum class status
        {
            none = 0,
            file,
            dir,
            err
        };
        status get_status(std::string path);
        int look_dir(std::string path, std::vector<std::string> &vec);
    }
}