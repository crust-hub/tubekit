#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <http-parser/http_parser.h>
#include "buffer/buffer.h"

namespace tubekit
{
    namespace request
    {
        class http_request
        {
        public:
            http_request(int socket_fd);
            ~http_request();
            void add_header(const std::string &key, const std::string &value);
            void add_to_body(const char *data, const size_t len);
            void add_chunk(const std::vector<char> &chunk);
            void set_url(const char *url, size_t url_len);
            http_parser *get_parser();
            void set_recv_over(bool recv_over);
            bool get_recv_over();
            void set_send_over(bool send_over);
            bool get_send_over();
            void set_processed(bool processed);
            bool get_processed();
            void set_write_eagain(bool write_eagain);
            bool get_write_eagain();

        public:
            std::string url;
            std::string method;
            std::map<std::string, std::vector<std::string>> headers;
            std::vector<char> body;
            std::vector<std::vector<char>> chunks;
            std::vector<char> data;
            buffer::buffer m_buffer;
            const size_t buffer_size{2048};
            char buffer[2048];
            int buffer_used_len; // effective content length in buffer
            std::string head_field_tmp;
            const int socket_fd;

        private:
            http_parser m_http_parser;
            bool recv_over;
            bool send_over;
            bool processed;
            bool write_eagain; // Used to mark whether EAGAIN occurred during writing. If so, when executing the code block for the next loop, the content in the buffer should be sent first before starting take from from m_buffer
        };
    }
}

std::ostream &operator<<(std::ostream &os, const tubekit::request::http_request &m_http_request);
