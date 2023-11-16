#include "task/websocket_task.h"

#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include "connection/connection.h"
#include "socket/socket_handler.h"

using namespace tubekit::task;
using namespace tubekit::socket;
using namespace tubekit::utility;
using namespace tubekit::connection;

http_parser_settings *websocket_task::settings = nullptr;

websocket_task::websocket_task(tubekit::socket::socket *m_socket) : task(m_socket)
{
    if (settings == nullptr)
    {
        settings = new http_parser_settings;
        settings->on_message_begin = [](http_parser *parser) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            http_method method = (http_method)parser->method;
            // TODO:
            return 0;
        };

        settings->on_url = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            // TODO:
            return 0;
        };

        settings->on_status = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            // TODO:
            return 0;
        };

        settings->on_header_value = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            // TODO:
            return 0;
        };

        settings->on_headers_complete = [](http_parser *parser) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            // TODO:
            return 0;
        };

        settings->on_body = [](http_parser *parser, const char *at, size_t length) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            // TODO:
            return 0;
        };

        settings->on_message_complete = [](http_parser *parser) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            // TODO:
            return 0;
        };

        settings->on_chunk_header = [](http_parser *parser) -> auto
        {
            connection::websocket_connection *t_websocket_connection = static_cast<connection::websocket_connection *>(parser->data);
            // TODO:
            return 0;
        };

        settings->on_chunk_complete = [](http_parser *parser) -> auto
        {
            // TODO:
            return 0;
        };
    }
}

websocket_task::~websocket_task()
{
    destroy();
}

void websocket_task::destroy()
{
}

void websocket_task::run()
{
    if (nullptr == m_data)
    {
        return;
    }
    socket::socket *socket_ptr = static_cast<socket::socket *>(m_data);

    if (!socket_ptr->close_callback)
    {
        socket_ptr->close_callback = [socket_ptr]() {
        };
    }

    // get connection layer instance
    connection::websocket_connection *t_websocket_connection = (connection::websocket_connection *)singleton<connection_mgr>::instance()->get(socket_ptr);

    if (nullptr == t_websocket_connection || t_websocket_connection->is_close())
    {
        if (t_websocket_connection)
        {
            singleton<connection_mgr>::instance()->remove(socket_ptr);
        }
        singleton<socket_handler>::instance()->remove(socket_ptr);
        return;
    }

    // 如果还没有连接则应该处理HTTP请求，如果没有Upgrade不是websocket则拒接socket连接
    // 处理好HTTP请求后，响应websocket连接请求响应，然后进入websocket已经连接状态
    // websocket连接状态将接收的内容收入到buffer中
    // 然后从buffer中尝试解析
    // 使用handler函数处理websocket报文
    // 如果待发送buffer中内容则进行发送，参考tcp stream
    // 需要封装发送websocket内容
    // 手动关闭连接之前里应当发送一下websocket关闭协议
}
