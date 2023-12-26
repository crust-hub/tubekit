#include "app/stream_app.h"
#include "proto_res/proto_cmd.pb.h"
#include "proto_res/proto_example.pb.h"
#include "proto_res/proto_message_head.pb.h"
#include <tubekit-log/logger.h>
#include <string>
#include <set>
#include "thread/mutex.h"
#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include "socket/socket.h"
#include "socket/socket_handler.h"

using tubekit::app::stream_app;
using tubekit::connection::connection_mgr;
using tubekit::connection::stream_connection;
using tubekit::socket::socket;
using tubekit::socket::socket_handler;
using tubekit::utility::singleton;

namespace tubekit::app
{
    std::set<void *> global_player;
    tubekit::thread::mutex global_player_mutex;
}

static int process_protocol(tubekit::connection::stream_connection &m_stream_connection, ProtoPackage &package)
{
    // EXAMPLE_REQ
    if (package.cmd() == ProtoCmd::EXAMPLE_REQ)
    {
        ProtoExampleReq exampleReq;
        if (exampleReq.ParseFromString(package.body()))
        {
            LOG_ERROR("%s", exampleReq.testcontext().c_str());
            // std::cout << exampleReq.testcontext() << std::endl;
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
    return 0;
}

void stream_app::on_stop()
{
    LOG_ERROR("stream_app::on_stop()");
}

void stream_app::on_tick()
{
    // LOG_ERROR("stream_app::on_tick()");
}

void stream_app::process_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    using tubekit::app::global_player;
    using tubekit::app::global_player_mutex;
    uint64_t all_data_len = m_stream_connection.m_recv_buffer.can_readable_size();
    char *all_data_buffer = new char[all_data_len];
    m_stream_connection.m_recv_buffer.copy_all(all_data_buffer, all_data_len);
    uint64_t offset = 0;

    do
    {
        char *tmp_buffer = all_data_buffer + offset;
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

    delete[] all_data_buffer;
}

void stream_app::on_close_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    using tubekit::app::global_player;
    using tubekit::app::global_player_mutex;
    global_player_mutex.lock();
    global_player.erase(m_stream_connection.get_socket_ptr());
    LOG_ERROR("player online %d", global_player.size());
    global_player_mutex.unlock();
}

void stream_app::on_new_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    using tubekit::app::global_player;
    using tubekit::app::global_player_mutex;
    global_player_mutex.lock();
    global_player.insert(m_stream_connection.get_socket_ptr());
    LOG_ERROR("player online %d", global_player.size());
    global_player_mutex.unlock();
}
