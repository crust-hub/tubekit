#include <string>
#include "workflow.h"
#include "utility/singleton_template.h"
#include "log/logger.h"
#include "plugin.h"
#include "plugin_loader.h"
#include "engine/context.h"

using namespace tubekit::engine;
using namespace tubekit::xml;
using namespace tubekit::log;
using namespace tubekit::utility;

workflow::~workflow()
{
    //删除所有pipline
    for (auto it = m_works.begin(); it != m_works.end(); ++it)
    {
        delete it->second;
        it->second = nullptr;
    }
    m_works.clear();
}

bool workflow::load(const std::string &path)
{
    document doc;
    doc.load_file(path);
    element root = doc.parse();
    for (auto it = root.begin(); it != root.end(); ++it)
    {
        std::string &&name = std::move(it->attr("name"));
        std::string &&flag = std::move(it->attr("switch"));
        work *new_work = new work;
        new_work->set_name(name);
        if (flag == "on")
        {
            new_work->set_switch(true);
        }
        else if (flag == "off")
        {
            new_work->set_switch(false);
        }
        else
        {
            return false;
        }
        if (!load_plugin(*new_work, (*it)))
        {
            return false;
        }
        m_works[name] = new_work;
    }
    return true;
}

bool workflow::run(const std::string &work, const std::string &input, std::string &output)
{
    auto it = m_works.find(work);
    if (it == m_works.end())
    {
        return false;
    }
    if (!it->second->get_switch())
    {
        return false;
    }
    context ctx;
    ctx.set<std::string>("input", input);
    if (!it->second->run(ctx))
    {
        return false;
    }
    output = ctx.ref<std::string>("output");
    return true;
}

bool workflow::load_plugin(work &work, tubekit::xml::element &el)
{
    for (auto it = el.begin(); it != el.end(); it++)
    {
        if (it->name() != "plugin")
        {
            return false;
        }
        const std::string &&name = std::move(it->attr("name"));
        plugin *(*create)() = (plugin * (*)()) singleton_template<plugin_loader>::instance()->get(name, "create");
        plugin *new_plugin = create();
        new_plugin->set_name(name);
        const std::string &&flag = std::move(it->attr("switch"));
        if (flag == "on")
        {
            new_plugin->set_switch(true);
        }
        else if (flag == "off")
        {
            new_plugin->set_switch(false);
        }
        else
        {
            return false;
        }
        work.append(new_plugin);
    }
    return true;
}