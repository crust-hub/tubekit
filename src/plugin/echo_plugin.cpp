#include <string>
#include <iostream>

#include "echo_plugin.h"

using namespace std;
using namespace tubekit::engine;
using namespace tubekit::plugin;

echo_plugin::echo_plugin() : plugin()
{
}

echo_plugin::~echo_plugin()
{
}

bool echo_plugin::run(context &ctx)
{
    std::string &input = ctx.ref<std::string>("input");
    // cout << input << endl;
    ctx.ref<std::string>("output") = input + " is echo plugin run!";
    return true;
}

tubekit::engine::plugin *create()
{
    return new echo_plugin;
}

void destory(tubekit::engine::plugin *p)
{
    delete p;
    p = nullptr;
}
