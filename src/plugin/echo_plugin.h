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
