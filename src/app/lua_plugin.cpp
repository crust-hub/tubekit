#include "app/lua_plugin.h"
#include <string>
#include <iostream>

using std::string;
using tubekit::app::lua_plugin;

void lua_plugin::on_init()
{
    // init lua vm
    lua_state = luaL_newstate();
    luaL_openlibs(lua_state);
    // exe lua file
    string filename = "./lua/init.lua";
    int isok = luaL_dofile(lua_state, filename.data());

    if (isok == 0)
    {
        // succ
    }
    else
    {
        // std::cout << "err " << lua_tostring(luaState, -1) << std::endl;
    }
}

void lua_plugin::on_exit()
{
    // close lua vm
    if (lua_state)
        lua_close(lua_state);
}

lua_plugin::lua_plugin() : lua_state(nullptr)
{
}
