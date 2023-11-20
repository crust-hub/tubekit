#include "app/websocket_app.h"
#include <vector>
#include <tubekit-log/logger.h>
#include "utility/singleton.h"
#include "connection/connection_mgr.h"
#include <arpa/inet.h>

using namespace tubekit::app;
using namespace tubekit::utility;
using namespace tubekit::connection;

struct WebSocketFrame
{
    uint8_t fin;
    uint8_t opcode;
    uint8_t mask;
    uint64_t payloadLength;
    std::vector<uint8_t> maskingKey;
    std::string payloadData;
};

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
    m_websocket_connection.m_recv_buffer.copy_all(data, all_data_len);
    size_t index = 0;

    while (true)
    {
        size_t start_index = index;

        WebSocketFrame frame;

        frame.fin = (data[index] & 0x80) != 0;
        frame.opcode = data[index] & 0x0F;
        index++;
        if (index >= all_data_len)
        {
            LOG_ERROR("index[%llu] >= all_data_len[%llu]", index, all_data_len);
            break;
        }

        frame.mask = (data[index] & 0x80) != 0;
        frame.payloadLength = data[index] & 0x7F;
        index++;
        if (index >= all_data_len)
        {
            LOG_ERROR("index[%llu] >= all_data_len[%llu]", index, all_data_len);
            break;
        }

        if (frame.payloadLength == 126)
        {
            frame.payloadLength = 0;
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
            frame.payloadLength = tmp;
            index += 2;
        }
        else if (frame.payloadLength == 127)
        {
            frame.payloadLength = 0;
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
            frame.payloadLength = ntohl(tmp);
            frame.payloadLength = frame.payloadLength << 32;
            ph = (u_char *)&tmp;
            *ph++ = data[index++];
            *ph++ = data[index++];
            *ph++ = data[index++];
            *ph++ = data[index++];
            tmp = ntohl(tmp);
            frame.payloadLength = frame.payloadLength | tmp;
        }

        if (frame.payloadLength == 0)
        {
            return;
        }

        if (frame.mask)
        {
            if (index + 3 >= all_data_len)
            {
                LOG_ERROR("index[%llu] >= all_data_len[%llu]", index + 3, all_data_len);
                break;
            }
            frame.maskingKey = {data[index], data[index + 1], data[index + 2], data[index + 3]};
            index += 4;
        }
        // payload data [data+index,data+index+frame.payloadLength]
        if (index >= all_data_len)
        {
            LOG_ERROR("index[%llu] >= all_data_len[%llu]", index, all_data_len);
            break;
        }
        if (index - 1 + frame.payloadLength >= all_data_len)
        {
            LOG_ERROR("index - 1 + frame.payloadLength=[%llu] >= all_data_len[%llu]", index - 1 + frame.payloadLength, all_data_len);
            break;
        }
        std::string payloadData(data + index, frame.payloadLength);
        if (frame.mask)
        {
            for (size_t i = 0; i < payloadData.size(); ++i)
            {
                payloadData[i] ^= frame.maskingKey[i % 4];
            }
        }
        frame.payloadData = std::move(payloadData);
        websocket_app::send_packet(m_websocket_connection, frame.payloadData.c_str(), frame.payloadLength, false);
        // frame.payloadData.push_back(0);
        // LOG_ERROR("%s", frame.payloadData.c_str());
        m_websocket_connection.m_recv_buffer.read_ptr_move_n(index - start_index + frame.payloadLength);
        index += frame.payloadLength;
    }
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
    size_t messageLength = data_len;
    std::vector<uint8_t> frame;
    frame.push_back(opcode);

    if (messageLength <= 125)
    {
        frame.push_back(static_cast<uint8_t>(messageLength));
    }
    else if (messageLength <= 0xFFFF)
    {
        frame.push_back(126);
        frame.push_back((messageLength >> 8) & 0xFF);
        frame.push_back(messageLength & 0xFF);
    }
    else
    {
        frame.push_back(127);
        for (int i = 7; i >= 0; --i)
        {
            frame.push_back((messageLength >> (8 * i)) & 0xFF);
        }
    }

    frame.insert(frame.end(), data, data + data_len);

    if (!use_safe)
    {
        return m_websocket_connection.send((const char *)frame.data(), frame.size());
    }
    return singleton<connection_mgr>::instance()->safe_send(m_websocket_connection.get_socket_ptr(), (const char *)frame.data(), frame.size());
}
