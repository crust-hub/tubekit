#include "app/http_app.h"
#include "utility/fs.h"
#include <string>
#include <vector>

using std::string;
using std::vector;
using tubekit::app::http_app;
using tubekit::request::http_request;
namespace fs = tubekit::fs;

class html_loader
{
public:
    static string load(string body)
    {
        static string frame1 = "<!DOCTYPE html>\
                        <html>\
                        <head>\
                        <title></title>\
                        </head>\
                        <body>";
        static string frame2 = "</body>\
                                </html>";
        return frame1 + body + frame2;
    }
    static string a_tag(string url, string text)
    {
        string frame = "<a href=\"" + url + "\">" + text + "</a></br>";
        return frame;
    }
};

void http_app::process_request(tubekit::request::http_request &m_http_request)
{
    m_http_request.m_buffer.set_limit_max(1024 * 1000);
    // load callback
    m_http_request.destory_callback = [](http_request &m_request) -> void
    {
        if (m_request.ptr)
        {
            FILE *file = (FILE *)m_request.ptr;
            ::fclose(file);
            m_request.ptr = nullptr;
        }
    };
    m_http_request.process_callback = [](http_request &request) -> void
    {
        string url = request.url;
        auto find_res = url.find("..");
        if (std::string::npos != find_res)
        {
            request.set_response_end(true);
            return;
        }
        const string prefix = "/mnt/c/Users/gaowanlu/Desktop/MyProject/tubekit";
        const string path = prefix + url;
        auto type = fs::get_status(path);
        if (type == fs::status::dir)
        {
            request.ptr = nullptr;
            const char *response = "HTTP/1.1 200 OK\r\nServer: tubekit\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
            request.m_buffer.write(response, strlen(response));
            //  generate dir list
            vector<string> a_tags;
            vector<string> dir_contents;
            if (0 == fs::look_dir(path, dir_contents))
            {
                if (url != "/")
                {
                    url += "/";
                }
                for (size_t i = 0; i < dir_contents.size(); ++i)
                {
                    a_tags.push_back(html_loader::a_tag(url + dir_contents[i], url + dir_contents[i]));
                }
            }
            string body;
            for (const auto &a_tag : a_tags)
            {
                body += a_tag;
            }
            string html = html_loader::load(body);
            request.m_buffer.write(html.c_str(), html.size());
            request.set_response_end(true);
            return;
        }
        else if (type == fs::status::file)
        {
            const char *response = "HTTP/1.1 200 OK\r\nServer: tubekit\r\nContent-Type: text/text; charset=UTF-8\r\n\r\n";
            request.m_buffer.write(response, strlen(response));
            request.ptr = nullptr;
            request.ptr = ::fopen(path.c_str(), "r");
            if (request.ptr == nullptr)
            {
                request.set_response_end(true);
                return;
            }
            // Write when the contents of the buffer have been sent write_end_callback will be executed,
            // and the response must be set response_end to true, then write after write_end_callback will be continuously recalled
            request.write_end_callback = [](http_request &m_request) -> void
            {
                char buf[1024] = {0};
                int len = 0;
                len = ::fread(buf, sizeof(char), 1024, (FILE *)m_request.ptr);
                if (len > 0)
                {
                    m_request.m_buffer.write(buf, len);
                }
                else
                {
                    m_request.set_response_end(true);
                }
            };
            return;
        }
        const char *response = "HTTP/1.1 404 Not Found\r\nServer: tubekit\r\nContent-Type: text/text; charset=UTF-8\r\n\r\n";
        request.m_buffer.write(response, strlen(response));
        request.set_response_end(true);
    };
}
