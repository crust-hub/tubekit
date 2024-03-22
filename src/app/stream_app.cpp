#include "app/stream_app.h"
#include "proto_res/proto_cmd.pb.h"
#include "proto_res/proto_example.pb.h"
#include "proto_res/proto_message_head.pb.h"
#include <tubekit-log/logger.h>
#include <string>
#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include "socket/socket.h"
#include "socket/socket_handler.h"
#include "app/lua_plugin.h"

using tubekit::app::stream_app;
using tubekit::connection::connection_mgr;
using tubekit::connection::stream_connection;
using tubekit::socket::socket;
using tubekit::socket::socket_handler;
using tubekit::utility::singleton;

namespace tubekit::app
{
    std::set<uint64_t> stream_app::global_player{};
    tubekit::thread::mutex stream_app::global_player_mutex;
};

static int process_protocol(tubekit::connection::stream_connection &m_stream_connection, ProtoPackage &package)
{
    // EXAMPLE_REQ
    if (package.cmd() == ProtoCmd::CS_REQ_EXAMPLE)
    {
        ProtoCSReqExample exampleReq;
        if (exampleReq.ParseFromString(package.body()))
        {
            // LOG_ERROR("%s", exampleReq.testcontext().c_str());
            ProtoPackage message;
            ProtoCSResExample exampleRes;
            exampleRes.set_testcontext(exampleReq.testcontext());
            message.set_cmd(ProtoCmd::CS_RES_EXAMPLE);
            // pong package
            std::string body_str;
            body_str.resize(exampleReq.ByteSizeLong());
            exampleReq.SerializeToString(&body_str);
            message.set_body(body_str);
            std::string data;
            message.SerializeToString(&data);
            bool send_res = m_stream_connection.send(data.c_str(), data.size());
            if (!send_res)
            {
                LOG_ERROR("send example res failed");
            }
        }
        else
        {
            return -1;
        }
        return 0;
    }
    return -1;
}

int stream_app::on_init()
{
    LOG_ERROR("stream_app::on_init()");
    singleton<app::lua_plugin>::instance()->on_init();
    return 0;
}

void stream_app::on_stop()
{
    LOG_ERROR("stream_app::on_stop()");
    singleton<app::lua_plugin>::instance()->on_exit();
}

void stream_app::on_tick()
{
    singleton<app::lua_plugin>::instance()->on_tick();
    // LOG_ERROR("stream_app::on_tick()");
}

void stream_app::process_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    uint64_t all_data_len = m_stream_connection.m_recv_buffer.can_readable_size();
    if (0 == all_data_len)
    {
        return;
    }

    const char *all_data_buffer = m_stream_connection.m_recv_buffer.force_get_read_ptr();
    uint64_t offset = 0;

    do
    {
        const char *tmp_buffer = all_data_buffer + offset;
        uint64_t data_len = all_data_len - offset;
        if (data_len == 0)
        {
            break;
        }

        ProtoPackage protoPackage;
        if (!protoPackage.ParseFromArray(tmp_buffer, data_len))
        {
            // std::cout << "protoPackage.ParseFromArray failed" << std::endl;
            break;
        }

        if (0 != process_protocol(m_stream_connection, protoPackage))
        {
            // std::cout << "process_protocol failed" << std::endl;
            m_stream_connection.mark_close();
            m_stream_connection.m_recv_buffer.clear();
            break;
        }
        // std::cout << "datalen " << data_len << " package size " << protoPackage.ByteSizeLong() << std::endl;
        offset += protoPackage.ByteSizeLong();

    } while (true);

    if (!m_stream_connection.m_recv_buffer.read_ptr_move_n(offset))
    {
        m_stream_connection.mark_close();
    }
}

void stream_app::on_close_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    global_player_mutex.lock();
    global_player.erase(m_stream_connection.get_gid());
    LOG_ERROR("player online %d, close_connection[%llu]", global_player.size(), m_stream_connection.get_gid());
    global_player_mutex.unlock();
}

void stream_app::on_new_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    global_player_mutex.lock();
    global_player.insert(m_stream_connection.get_gid());
    LOG_ERROR("player online %d new_connection[%llu]", global_player.size(), m_stream_connection.get_gid());
    global_player_mutex.unlock();
}
