#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <http-parser/http_parser.h>
#include <tubekit-buffer/buffer.h>

#include "connection/connection.h"
#include "socket/socket.h"

namespace tubekit
{
    namespace connection
    {
        class http_connection : public connection
        {
        public:
            http_connection(tubekit::socket::socket *socket_ptr);
            ~http_connection();
            void add_header(const std::string &key, const std::string &value);
            void add_to_body(const char *data, const size_t len);
            void add_chunk(const std::vector<char> &chunk);
            void set_url(const char *url, size_t url_len);
            http_parser *get_parser();
            void set_recv_end(bool recv_end);
            bool get_recv_end();
            void set_process_end(bool process_end);
            bool get_process_end();
            void set_response_end(bool response_end);
            bool get_response_end();
            bool set_everything_end(bool everything_end);
            bool get_everything_end();

        public:
            std::string url;
            std::string method;
            std::map<std::string, std::vector<std::string>> headers;
            std::vector<char> body;
            std::vector<std::vector<char>> chunks;
            std::vector<char> data;
            buffer::buffer m_buffer;
            const size_t buffer_size{10240};
            char buffer[10240];
            int buffer_used_len; // effective content length in buffer
            int buffer_start_use;
            std::string head_field_tmp;
            std::function<void(http_connection &connection)> process_callback;
            std::function<void(http_connection &connection)> write_end_callback;
            std::function<void(http_connection &connection)> destory_callback;
            void *ptr{nullptr};

        private:
            http_parser m_http_parser;
            bool recv_end;
            bool process_end;
            bool response_end;
            bool everything_end;
            tubekit::socket::socket *socket_ptr;
        };
    }
}

std::ostream &operator<<(std::ostream &os, const tubekit::connection::http_connection &m_http_connection);
