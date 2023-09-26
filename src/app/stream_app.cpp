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
#include <vector>

using tubekit::app::stream_app;
using tubekit::connection::connection_mgr;
using tubekit::connection::stream_connection;
using tubekit::utility::singleton;

namespace tubekit::app
{
    std::set<void *> global_player;
    tubekit::thread::mutex global_player_mutex;
}

void stream_app::process_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    using tubekit::app::global_player;
    using tubekit::app::global_player_mutex;
    uint64_t data_len = m_stream_connection.m_recv_buffer.can_readable_size();
    char *tmp_buffer = new char[data_len];
    m_stream_connection.m_recv_buffer.copy_all(tmp_buffer, data_len);
    m_stream_connection.m_recv_buffer.clear();

    std::vector<void *> vec;
    global_player_mutex.lock();
    for (auto socket_ptr : global_player)
    {
        if (socket_ptr != m_stream_connection.get_socket_ptr())
        {
            vec.push_back(socket_ptr);
        }
    }
    global_player_mutex.unlock();
    for (auto socket_ptr : vec)
    {
        bool b_ret = singleton<connection_mgr>::instance()->safe_send(socket_ptr, tmp_buffer, data_len);
    }
    delete tmp_buffer;
    // m_stream_connection.mark_close();
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
