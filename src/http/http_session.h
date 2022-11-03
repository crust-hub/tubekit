#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>

namespace tubekit
{
    namespace http
    {
        class session
        {
        public:
            session();
            ~session();
            void add_header(const char *key, const size_t key_len, const char *value, const size_t value_len);
            void add_to_body(const char *data, const size_t len);
            void add_chunk(const std::vector<char> &chunk);
            void set_url(const char *url, size_t url_len);

        public:
            std::string url;
            std::multimap<std::string, std::string> headers;
            std::vector<char> body;
            std::vector<std::vector<char>> chunks;
            std::vector<char> data;
        };
    }
}

std::ostream &operator<<(std::ostream &os, const tubekit::http::session &m_session);