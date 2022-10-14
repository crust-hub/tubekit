#include <dlfcn.h>
#include <assert.h>
#include <string>
#include <cstdio>
#include <cstring>

#include "plugin_loader.h"
#include "system/system.h"
#include "utility/singleton_template.h"

using std::map;
using std::string;
using namespace tubekit::engine;
using namespace tubekit::system;
using namespace tubekit::utility;

plugin_loader::plugin_loader()
{
}

plugin_loader::~plugin_loader()
{
}

void plugin_loader::load(const std::string &plugin)
{
    assert(!plugin.empty());
    assert(m_plugins.find(plugin) == m_plugins.end()); //插件已存在map中
    string filename = singleton_template<system::system>::instance()->get_root_path() + "/plugin/" + plugin;
    //获得动态链接库句柄
    void *handle = dlopen(filename.c_str(), RTLD_GLOBAL | RTLD_LAZY);
    printf("load plugin %s %s\n", filename.c_str(), dlerror());
    assert(handle != nullptr);
    m_plugins[plugin] = handle; //存进map
}

void plugin_loader::unload(const std::string &plugin)
{
    assert(!plugin.empty());
    auto it = m_plugins.find(plugin);
    assert(it != m_plugins.end());
    dlclose(m_plugins[plugin]); //关闭指定句柄的动态链接库
    m_plugins.erase(it);        //从map中删除
}

void *plugin_loader::get(const string &plugin, const string &symbol)
{
    auto it = m_plugins.find(plugin);
    if (it == m_plugins.end())
    {
        load(plugin);
        //再次尝试查询
        it = m_plugins.find(plugin);
        assert(it != m_plugins.end());
    }
    void *func = dlsym(it->second, symbol.c_str()); //加载可执行函数
    assert(func != nullptr);
    return func;
}
