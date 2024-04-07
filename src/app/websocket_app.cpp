#include "app/websocket_app.h"
#include <vector>
#include <tubekit-log/logger.h>
#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include <arpa/inet.h>
#include "app/lua_plugin.h"

using namespace tubekit::app;
using namespace tubekit::utility;
using namespace tubekit::connection;

namespace tubekit::app
{
    std::set<uint64_t> websocket_app::global_player{};
    tubekit::thread::mutex websocket_app::global_player_mutex;
};

websocket_app::websocket_frame_type websocket_app::n_2_websocket_frame_type(uint8_t n)
{
    n = n & 0x0f;
    if (n == 0x1)
    {
        return websocket_frame_type::TEXT_FRAME;
    }
    else if (n == 0x2)
    {
        return websocket_frame_type::BINARY_FRAME;
    }
    else
    {
        if (n == 0x0)
        {
            return websocket_frame_type::CONTINUATION_FRAME;
        }
        else if (n >= 0x3 && n <= 0x7)
        {
            return websocket_frame_type::FURTHER_NON_CONTROL;
        }
        else if (n >= 0xb && n <= 0xf)
        {
            return websocket_frame_type::FURTHER_CONTROL;
        }
        else if (n == 0x8)
        {
            return websocket_frame_type::CONNECTION_CLOSE_FRAME;
        }
        else if (n == 0x9)
        {
            return websocket_frame_type::PING;
        }
        else if (n == 0xa)
        {
            return websocket_frame_type::PONG;
        }
    }
    return websocket_frame_type::ERROR;
}

uint8_t websocket_app::websocket_frame_type_2_n(websocket_frame_type type, uint8_t idx /*= 0x0*/)
{
    if (type == websocket_frame_type::TEXT_FRAME)
    {
        return 0x1;
    }
    else if (type == websocket_frame_type::BINARY_FRAME)
    {
        return 0x2;
    }
    else
    {
        if (type == websocket_frame_type::CONTINUATION_FRAME)
        {
            return 0x0;
        }
        else if (type == websocket_frame_type::FURTHER_NON_CONTROL && idx >= 0x0 && idx <= 0x4)
        {
            return 0x3 + idx;
        }
        else if (type == websocket_frame_type::FURTHER_CONTROL && idx >= 0x0 && idx <= 0x5)
        {
            return 0xb + idx;
        }
        else if (type == websocket_frame_type::CONNECTION_CLOSE_FRAME)
        {
            return 0x8;
        }
        else if (type == websocket_frame_type::PING)
        {
            return 0x9;
        }
        else if (type == websocket_frame_type::PONG)
        {
            return 0xa;
        }
    }
    return 0x8;
}

int websocket_app::on_init()
{
    LOG_ERROR("websocket_app::on_init()");
    singleton<app::lua_plugin>::instance()->on_init();
    return 0;
}

void websocket_app::on_stop()
{
    LOG_ERROR("websocket_app::on_stop()");
    singleton<app::lua_plugin>::instance()->on_exit();
}

void websocket_app::on_tick()
{
    singleton<app::lua_plugin>::instance()->on_tick();
    // LOG_ERROR("websocket_app::on_tick()");
}

void websocket_app::process_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    // LOG_ERROR("process_connection");
    uint64_t all_data_len = m_websocket_connection.m_recv_buffer.can_readable_size();
    if (all_data_len == 0)
    {
        // LOG_ERROR("all_data_len <= 0");
        return;
    }
    const char *data = m_websocket_connection.m_recv_buffer.force_get_read_ptr();

    size_t index = 0;

    while (true)
    {
        if (index >= all_data_len)
        {
            break;
        }

        size_t start_index = index;

        websocket_frame frame;
        uint8_t opcode = (uint8_t)data[index] & 0x0f;
        websocket_frame_type type = n_2_websocket_frame_type(opcode);

        if (type != websocket_frame_type::TEXT_FRAME && type != websocket_frame_type::BINARY_FRAME)
        {
            m_websocket_connection.mark_close();
            break;
        }

        frame.fin = (data[index] & 0x80) != 0;
        frame.opcode = data[index] & 0x0F;
        index++;
        if (index >= all_data_len)
        {
            LOG_ERROR("index[%llu] >= all_data_len[%llu]", index, all_data_len);
            break;
        }

        frame.mask = (data[index] & 0x80) != 0;
        frame.payload_length = data[index] & 0x7F;
        index++;
        if (index >= all_data_len)
        {
            LOG_ERROR("index[%llu] >= all_data_len[%llu]", index, all_data_len);
            break;
        }

        if (frame.payload_length == 126)
        {
            frame.payload_length = 0;
            if (index + 2 >= all_data_len)
            {
                LOG_ERROR("index[%llu] >= all_data_len[%llu]", index + 2, all_data_len);
                break;
            }
            uint16_t tmp = 0;
            u_char *ph;
            ph = (u_char *)&tmp;
            *ph++ = data[index];
            *ph++ = data[index + 1];
            tmp = ntohs(tmp);
            frame.payload_length = tmp;
            index += 2;
        }
        else if (frame.payload_length == 127)
        {
            frame.payload_length = 0;
            if (index + 8 >= all_data_len)
            {
                LOG_ERROR("index[%llu] >= all_data_len[%llu]", index + 8, all_data_len);
                break;
            }
            uint32_t tmp = 0;
            u_char *ph = (u_char *)&tmp;
            *ph++ = data[index++];
            *ph++ = data[index++];
            *ph++ = data[index++];
            *ph++ = data[index++];
            frame.payload_length = ntohl(tmp);
            frame.payload_length = frame.payload_length << 32;
            ph = (u_char *)&tmp;
            *ph++ = data[index++];
            *ph++ = data[index++];
            *ph++ = data[index++];
            *ph++ = data[index++];
            tmp = ntohl(tmp);
            frame.payload_length = frame.payload_length | tmp;
        }

        if (frame.payload_length == 0)
        {
            break;
        }

        if (frame.mask)
        {
            if (index + 4 >= all_data_len)
            {
                LOG_ERROR("index[%llu] >= all_data_len[%llu]", index + 3, all_data_len);
                break;
            }
            frame.masking_key = {(uint8_t)data[index], (uint8_t)data[index + 1], (uint8_t)data[index + 2], (uint8_t)data[index + 3]};
            index += 4;
        }
        // payload data [data+index,data+index+frame.payload_length]
        if (index >= all_data_len)
        {
            LOG_ERROR("index[%llu] >= all_data_len[%llu]", index, all_data_len);
            break;
        }
        if (index - 1 + frame.payload_length >= all_data_len)
        {
            // LOG_ERROR("index - 1 + frame.payload_length=[%llu] >= all_data_len[%llu]", index - 1 + frame.payload_length, all_data_len);
            break;
        }
        std::string payload_data(data + index, frame.payload_length);
        if (frame.mask)
        {
            for (size_t i = 0; i < payload_data.size(); ++i)
            {
                payload_data[i] ^= frame.masking_key[i % 4];
            }
        }
        frame.payload_data = std::move(payload_data);

        process_frame(m_websocket_connection, frame);

        m_websocket_connection.m_recv_buffer.read_ptr_move_n(index - start_index + frame.payload_length);
        index += frame.payload_length;
    }
}

void websocket_app::process_frame(tubekit::connection::websocket_connection &m_websocket_connection,
                                  websocket_frame &frame)
{
    // broadcast
    std::set<uint64_t> global_player_copy;
    global_player_mutex.lock();
    global_player_copy = global_player;
    global_player_mutex.unlock();

    uint8_t first_byte = 0x80 | websocket_frame_type_2_n(websocket_frame_type::TEXT_FRAME);

    for (auto player : global_player_copy)
    {
        websocket_app::send_packet(nullptr, first_byte, frame.payload_data.c_str(), frame.payload_length, player);
    }
}

void websocket_app::on_close_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    global_player_mutex.lock();
    global_player.erase(m_websocket_connection.get_gid());
    LOG_ERROR("player online %d, close_connection[%llu]", global_player.size(), m_websocket_connection.get_gid());
    global_player_mutex.unlock();
}

void websocket_app::on_new_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    global_player_mutex.lock();
    global_player.insert(m_websocket_connection.get_gid());
    LOG_ERROR("player online %d new_connection[%llu]", global_player.size(), m_websocket_connection.get_gid());
    global_player_mutex.unlock();
}

bool websocket_app::send_packet(tubekit::connection::websocket_connection *m_websocket_connection,
                                uint8_t first_byte,
                                const char *data,
                                size_t data_len,
                                uint64_t gid /*= 0*/)
{
    if (!data)
    {
        return false;
    }
    size_t message_length = data_len;
    std::vector<uint8_t> frame;
    frame.push_back(first_byte);

    if (message_length <= 125)
    {
        frame.push_back(static_cast<uint8_t>(message_length));
    }
    else if (message_length <= 0xFFFF)
    {
        frame.push_back(126);
        frame.push_back((message_length >> 8) & 0xFF);
        frame.push_back(message_length & 0xFF);
    }
    else
    {
        frame.push_back(127);
        for (int i = 7; i >= 0; --i)
        {
            frame.push_back((message_length >> (8 * i)) & 0xFF);
        }
    }

    frame.insert(frame.end(), data, data + data_len);

    if (0 == gid && m_websocket_connection)
    {
        return m_websocket_connection->send((const char *)frame.data(), frame.size());
    }
    else
    {
        bool res = false;
        singleton<connection_mgr>::instance()->if_exist(
            gid,
            [&res, &frame](uint64_t key, std::pair<tubekit::socket::socket *, tubekit::connection::connection *> value)
            {
                tubekit::connection::websocket_connection *p_wsconn = (tubekit::connection::websocket_connection *)(value.second);
                res = p_wsconn->send((const char *)frame.data(), frame.size());
            },
            nullptr);
        return res;
    }

    return false;
}
