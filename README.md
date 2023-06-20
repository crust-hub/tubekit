# Tubekit

The C++ TCP server framework based on the Reactor model continues to implement POSIX thread pool, Epoll, non blocking IO, object pool, log, socket network programming, support the dynamic library to implement custom protocol extensions, and use http parser to process http requests. Currently only supports Linux systems

## Get Start

prepare

```bash
$ apt install g++ cmake make
$ git clone https://github.com/crust-hub/tubekit.git
$ cmake .
$ make
```

Run

```bash
sudo bash ./run.sh
```

Stop

```bash
sudo bash ./kill.sh
```

## Http App

```cpp
#include "app/http_app.h"
#include <string>

using std::string;
using tubekit::app::http_app;
using tubekit::request::http_request;

void http_app::process_request(tubekit::request::http_request &m_http_request)
{
    // load callback
    m_http_request.process_callback = [](http_request &request) -> void
    {
        const string &url = request.url;
        auto find_res = url.find("..");
        if (find_res != std::string::npos)
        {
            request.set_response_end(true);
            return;
        }
        const string path = std::string("../src") + url;
        request.ptr = nullptr;
        request.ptr = fopen(path.c_str(), "r");
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
```

## Directory Structure

[Directory Structure Link](./doc/dir_detail.md)

## Third Party

[@http-parser](https://github.com/nodejs/http-parser)  
[@yazi](https://github.com/oldjun/yazi)(source of inspiration)
