#include "app/http_app.h"
#include <string>
#include <vector>
#include <filesystem>
#include <tubekit-log/logger.h>

#include "utility/mime_type.h"

using std::string;
using std::vector;
using tubekit::app::http_app;
using tubekit::connection::http_connection;
namespace fs = std::filesystem;
namespace utility = tubekit::utility;

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

void http_app::process_connection(tubekit::connection::http_connection &m_http_connection)
{
    m_http_connection.m_buffer.set_limit_max(512000);
    // load callback
    m_http_connection.destory_callback = [](http_connection &m_connection) -> void
    {
        if (m_connection.ptr)
        {
            FILE *file = (FILE *)m_connection.ptr;
            ::fclose(file);
            m_connection.ptr = nullptr;
        }
    };
    m_http_connection.process_callback = [](http_connection &connection) -> void
    {
        string url = connection.url;
        auto find_res = url.find("..");
        if (std::string::npos != find_res)
        {
            connection.set_response_end(true);
            return;
        }
        const string prefix = "/";

        fs::path t_path(prefix + url);

        if (fs::exists(t_path) && fs::status(t_path).type() == fs::file_type::regular)
        {
            std::string mime_type;
            try
            {
                mime_type = utility::mime_type::get_type(t_path.string());
            }
            catch (...)
            {
                mime_type = "application/octet-stream";
            }
            std::string response = "HTTP/1.1 200 OK\r\nServer: tubekit\r\n";
            response += "Content-Type: ";
            response += mime_type + "\r\n\r\n";
            try
            {
                connection.m_buffer.write(response.c_str(), response.size());
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR(e.what());
            }
            connection.ptr = nullptr;
            connection.ptr = ::fopen(t_path.c_str(), "r");
            if (connection.ptr == nullptr)
            {
                connection.set_response_end(true);
                return;
            }
            // Write when the contents of the buffer have been sent write_end_callback will be executed,
            // and the response must be set response_end to true, then write after write_end_callback will be continuously recalled
            connection.write_end_callback = [](http_connection &m_connection) -> void
            {
                char buf[204800] = {0};
                int len = 0;
                len = ::fread(buf, sizeof(char), 204800, (FILE *)m_connection.ptr);
                if (len > 0)
                {
                    try
                    {
                        m_connection.m_buffer.write(buf, len);
                    }
                    catch (const std::runtime_error &e)
                    {
                        LOG_ERROR(e.what());
                    }
                }
                else
                {
                    m_connection.set_response_end(true);
                }
            };
            return;
        }

        if (fs::exists(t_path) && fs::status(t_path).type() == fs::file_type::directory)
        {
            connection.ptr = nullptr;
            const char *response = "HTTP/1.1 200 OK\r\nServer: tubekit\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
            try
            {
                connection.m_buffer.write(response, strlen(response));
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR(e.what());
            }
            //  generate dir list
            vector<string> a_tags;
            for (const auto &dir_entry : fs::directory_iterator{t_path})
            {
                std::string sub_path = dir_entry.path().string().substr(prefix.size());
                a_tags.push_back(html_loader::a_tag(sub_path, sub_path));
            }
            string body;
            for (const auto &a_tag : a_tags)
            {
                body += a_tag;
            }
            string html = html_loader::load(body);
            try
            {
                connection.m_buffer.write(html.c_str(), html.size());
            }
            catch (const std::runtime_error &e)
            {
                LOG_ERROR(e.what());
            }
            connection.set_response_end(true);
            return;
        }

        const char *response = "HTTP/1.1 404 Not Found\r\nServer: tubekit\r\nContent-Type: text/text; charset=UTF-8\r\n\r\n";
        try
        {
            connection.m_buffer.write(response, strlen(response));
        }
        catch (const std::runtime_error &e)
        {
            LOG_ERROR(e.what());
        }
        connection.set_response_end(true);
    };
}
