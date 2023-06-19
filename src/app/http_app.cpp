#include "app/http_app.h"

using tubekit::app::http_app;
using tubekit::request::http_request;

void http_app::process_request(tubekit::request::http_request &m_http_request)
{
    // load callback
    m_http_request.process_callback = [](http_request &request) -> void
    {
        constexpr const char *path = "../src/app/http_app.cpp";
        request.ptr = nullptr;
        request.ptr = fopen(path, "r");
        if (request.ptr == nullptr)
        {
            request.set_response_end(true);
            return;
        }
        const char *response = "HTTP/1.1 200 OK\r\nServer: tubekit\r\nContent-Type: text/text;\r\n\r\n";
        request.m_buffer.write(response, strlen(response));
        // Write when the contents of the buffer have been sent write_end_callback will be executed,
        // and the response must be set response_end to true, then write after write_end_callback will be continuously recalled
        request.write_end_callback = [](http_request &m_request) -> void
        {
            // std::cout << "write_end_callback" << std::endl;
            char buf[1024] = {0};
            int len = 0;
            len = fread(buf, sizeof(char), 1024, (FILE *)m_request.ptr);
            if (len > 0)
            {
                m_request.m_buffer.write(buf, len);
            }
            else
            {
                m_request.set_response_end(true);
            }
        };
        request.destory_callback = [](http_request &m_request) -> void
        {
            // std::cout << "destory_callback" << std::endl;
            if (m_request.ptr)
            {
                FILE *ptr = (FILE *)m_request.ptr;
                fclose(ptr);
                m_request.ptr = nullptr;
            }
        };
    };
}