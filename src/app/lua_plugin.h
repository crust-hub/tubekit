#pragma once

extern "C"
{
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

namespace tubekit::app
{
    class lua_plugin
    {
    public:
        lua_plugin();
        void on_init();
        void on_exit();

        void exe_OnInit();
        void exe_OnExit();

        void mount();

    public:
        static int Logger(lua_State *lua_state);

    private:
        lua_State *lua_state;
    };
}