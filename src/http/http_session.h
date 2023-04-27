#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <http_parser.h>

namespace tubekit
{
    namespace http
    {
        class session
        {
        public:
            session(int socket_fd);
            ~session();
            void add_header(const std::string &key, const std::string &value);
            void add_to_body(const char *data, const size_t len);
            void add_chunk(const std::vector<char> &chunk);
            void set_url(const char *url, size_t url_len);
            http_parser *get_parser();
            void set_over(bool over);
            bool get_over();

        public:
            std::string url;
            std::string method;
            std::map<std::string, std::vector<std::string>> headers;
            std::vector<char> body;
            std::vector<std::vector<char>> chunks;
            std::vector<char> data;
            char *buffer;
            std::string head_field_tmp;
            const int buffer_size;
            const int socket_fd;
            bool over;

        private:
            http_parser *m_http_parser;
        };
    }
}

std::ostream &operator<<(std::ostream &os, const tubekit::http::session &m_session);