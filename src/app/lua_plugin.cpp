#include "app/lua_plugin.h"
#include <string>
#include <iostream>

#include "server/server.h"
#include "utility/singleton.h"
#include "tubekit-log/logger.h"

using std::string;
using tubekit::app::lua_plugin;
using tubekit::server::server;
using tubekit::utility::singleton;

lua_plugin::lua_plugin() : lua_state(nullptr)
{
}

void lua_plugin::on_init()
{
    // init lua vm
    lua_state = luaL_newstate();
    luaL_openlibs(lua_state);
    // exe lua file
    string filename = singleton<server::server>::instance()->get_lua_dir() + "/init.lua";
    int isok = luaL_dofile(lua_state, filename.data());

    if (isok == 0)
    {
        LOG_ERROR("init.lua load succ");
        mount();
        exe_OnInit();
    }
    else
    {
        LOG_ERROR("init.lua load failed, %s", lua_tostring(lua_state, -1));
    }
    LOG_ERROR("lua_plugin on_init");
}

void lua_plugin::on_exit()
{
    // close lua vm
    if (lua_state)
    {
        exe_OnExit();
        lua_close(lua_state);
    }
    LOG_ERROR("lua_plugin on_exit");
}

void lua_plugin::exe_OnInit()
{
    lua_getglobal(lua_state, "OnInit");
    int isok = lua_pcall(lua_state, 0, 0, 0);
    if (0 != isok)
    {
        LOG_ERROR("exe_OnInit failed %s", lua_tostring(lua_state, -1));
    }
}

void lua_plugin::exe_OnExit()
{
    lua_getglobal(lua_state, "OnExit");
    int isok = lua_pcall(lua_state, 0, 0, 0);
    if (0 != isok)
    {
        LOG_ERROR("exe_OnExit failed %s", lua_tostring(lua_state, -1));
    }
}

void lua_plugin::mount()
{
    static luaL_Reg lulibs[] = {
        {"Logger", Logger},
        {NULL, NULL}};
    luaL_newlib(lua_state, lulibs);
    lua_setglobal(lua_state, "tubekit");
}

int lua_plugin::Logger(lua_State *lua_state)
{
    int num = lua_gettop(lua_state);
    if (num != 1)
    {
        lua_pushinteger(lua_state, -1);
        return -1;
    }
    if (lua_isstring(lua_state, 1) == 0)
    {
        lua_pushinteger(lua_state, -1);
        return -1;
    }
    size_t len = 0;
    const char *type = lua_tolstring(lua_state, 1, &len);
    string str(type, len);
    LOG_ERROR("%s", str.data());
    lua_pushinteger(lua_state, 0);
    return 0;
}