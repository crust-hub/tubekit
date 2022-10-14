#pragma once
#include <string>
#include <map>

#include "work.h"
#include "xml/element.h"
#include "xml/document.h"

namespace tubekit
{
    namespace engine
    {
        class workflow
        {
        public:
            workflow() = default;
            ~workflow();
            bool load(const std::string &path);
            bool run(const std::string &work, const std::string &input, std::string &output);

        private:
            bool load_plugin(work &work, tubekit::xml::element &el);

        private:
            std::map<std::string, work *> m_works;
        };
    }
}