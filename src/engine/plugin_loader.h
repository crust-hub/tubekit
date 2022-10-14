#pragma once

#include <string>
#include <map>

namespace tubekit
{
    namespace engine
    {
        class plugin_loader
        {
        public:
            plugin_loader();
            ~plugin_loader();
            void load(const std::string &plugin);
            void unload(const std::string &plugin);
            void *get(const std::string &plugin, const std::string &symbol);

        private:
            std::map<std::string, void *> m_plugins;
        };
    }
}