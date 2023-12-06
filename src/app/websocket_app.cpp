#include "app/websocket_app.h"
#include <vector>
#include <tubekit-log/logger.h>
#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include <arpa/inet.h>

using namespace tubekit::app;
using namespace tubekit::utility;
using namespace tubekit::connection;

struct websocket_frame
{
    uint8_t fin;
    uint8_t opcode;
    uint8_t mask;
    uint64_t payload_length;
    std::vector<uint8_t> masking_key;
    std::string payload_data;
};

enum class websocket_frame_type
{
    CONNECTION_CLOSE_FRAME = 0,
    TEXT_FRAME = 1,
    BINARY_FRAME = 2,
    PONG = 3,
    PING = 4,
    CONTINUATION_FRAME = 5,
    ERROR = 6
};

int websocket_app::on_init()
{
    LOG_ERROR("websocket_app::on_init()");
    return 0;
}

void websocket_app::on_stop()
{
    LOG_ERROR("websocket_app::on_stop()");
}

void websocket_app::on_tick()
{
    // LOG_ERROR("websocket_app::on_tick()");
}

void websocket_app::process_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    LOG_ERROR("process_connection");
    uint64_t all_data_len = m_websocket_connection.m_recv_buffer.can_readable_size();
    if (all_data_len <= 0)
    {
        LOG_ERROR("all_data_len <= 0");
        return;
    }
    char *data = new (std::nothrow) char[all_data_len];
    if (!data)
    {
        return;
    }
    all_data_len = m_websocket_connection.m_recv_buffer.copy_all(data, all_data_len);
    size_t index = 0;

    while (true)
    {
        if (index >= all_data_len)
        {
            break;
        }

        size_t start_index = index;

        websocket_frame frame;
        websocket_frame_type type = websocket_frame_type::ERROR;
        switch ((uint8_t)data[index])
        {
        case 0x81:
        {
            type = websocket_frame_type::TEXT_FRAME;
            break;
        }
        case 0x82:
        {
            type = websocket_frame_type::BINARY_FRAME;
            break;
        }
        case 0x88:
        {
            type = websocket_frame_type::CONNECTION_CLOSE_FRAME;
            break;
        }
        case 0x89:
        {
            type = websocket_frame_type::PING;
            break;
        }
        default:
        {
            if (data[index] >= 0x00 && data[index] <= 0x7F)
            {
                type = websocket_frame_type::CONTINUATION_FRAME;
            }
            break;
        }
        }

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
            LOG_ERROR("index - 1 + frame.payload_length=[%llu] >= all_data_len[%llu]", index - 1 + frame.payload_length, all_data_len);
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

        // broadcast
        singleton<connection_mgr>::instance()->for_each(
            [&frame](connection::connection &conn) -> void
            {
                websocket_connection *ptr_conn = static_cast<websocket_connection *>(&conn);
                websocket_app::send_packet(*ptr_conn, frame.payload_data.c_str(), frame.payload_length, false);
            });

        // websocket_app::send_packet(m_websocket_connection, frame.payload_data.c_str(), frame.payload_length, false);
        // frame.payload_data.push_back(0);
        // LOG_ERROR("%s", frame.payload_data.c_str());
        m_websocket_connection.m_recv_buffer.read_ptr_move_n(index - start_index + frame.payload_length);
        index += frame.payload_length;
    }

    delete[] data;
}

void websocket_app::on_close_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    LOG_ERROR("on_close_connection");
}

void websocket_app::on_new_connection(tubekit::connection::websocket_connection &m_websocket_connection)
{
    LOG_ERROR("on_new_connection");
}

bool websocket_app::send_packet(tubekit::connection::websocket_connection &m_websocket_connection, const char *data, size_t data_len, bool use_safe)
{
    if (!data)
    {
        return false;
    }
    uint8_t opcode = 0x81;
    size_t message_length = data_len;
    std::vector<uint8_t> frame;
    frame.push_back(opcode);

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

    if (!use_safe)
    {
        return m_websocket_connection.send((const char *)frame.data(), frame.size());
    }
    return singleton<connection_mgr>::instance()->safe_send(m_websocket_connection.get_socket_ptr(), (const char *)frame.data(), frame.size());
}
