#include "utility/fs.h"

using std::string;
using std::vector;
using namespace tubekit;

fs::status fs::get_status(string path)
{
    if (access(path.c_str(), F_OK) != 0)
    {
        return fs::status::none;
    }
    struct stat file_info;
    int result = stat(path.c_str(), &file_info);
    if (result == 0)
    {
        if (S_ISREG(file_info.st_mode))
        {
            return fs::status::file;
        }
        else if (S_ISDIR(file_info.st_mode))
        {
            return fs::status::dir;
        }
        else
        {
            return fs::status::none;
        }
    }
    return fs::status::err;
}

int fs::look_dir(string path, vector<string> &vec)
{
    if (get_status(path) != status::dir)
    {
        return -1;
    }
    DIR *directory = opendir(path.c_str());
    if (directory == NULL)
    {
        return -1;
    }
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        vec.push_back(entry->d_name);
    }
    closedir(directory);
    return 0;
}