# libengine

WARN: Temporarily stop using

```cpp
//echo_plugin.h
#pragma once
#include "engine/context.h"
#include "engine/plugin.h"

namespace tubekit
{
    namespace plugin
    {
        class echo_plugin : public tubekit::engine::plugin
        {
        public:
            echo_plugin();
            virtual ~echo_plugin();
            virtual bool run(tubekit::engine::context &ctx);
        };
    }
}

extern "C"
{
    tubekit::engine::plugin *create();
    void destory(tubekit::engine::plugin *p);
}
```

```cpp
//echo_plugin.cpp
#include <string>
#include <iostream>

#include "plugin/echo_plugin.h"

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
```

```cmake
#CMakeLists.txt
message('cmake plugin')

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/plugin/)
add_library(echoplugin SHARED ${PROJECT_SOURCE_DIR}/src/plugin/echo_plugin.cpp  ${PROJECT_SOURCE_DIR}/src/engine/plugin.cpp)
```

```cpp
//  init workflow
// engine::workflow *work = singleton<engine::workflow>::instance();
// work->load(get_root_path() + "/config/workflow.xml");
```