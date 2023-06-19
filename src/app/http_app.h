#pragma once
#include "request/http_request.h"

namespace tubekit
{
    namespace app
    {
        class http_app
        {
        public:
            static void process_request(tubekit::request::http_request &m_http_request);
        };
    }
}