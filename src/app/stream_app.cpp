#include "app/stream_app.h"
#include "proto_res/proto_cmd.pb.h"
#include "proto_res/proto_example.pb.h"
#include "proto_res/proto_message_head.pb.h"
#include <tubekit-log/logger.h>
#include <string>

using tubekit::app::stream_app;
using tubekit::connection::stream_connection;

void stream_app::process_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    m_stream_connection.m_recv_buffer.clear();

    // ProtoMessageHead protoMessageHead;
    // protoMessageHead.set_cmd(0);

    // std::string buffer;

    // ProtoExampleReq protoExampleReq;
    // protoExampleReq.set_testcontext("hello world");

    // protoMessageHead.set_bodylen(buffer.size());

    // protoMessageHead.SerializeToString(&buffer);
    // m_stream_connection.send(buffer.data(), buffer.size());
    // buffer.clear();

    // protoExampleReq.SerializeToString(&buffer);

    // m_stream_connection.send(buffer.data(), buffer.size());
    // buffer.clear();
    // m_stream_connection.mark_close();
}

void stream_app::on_close_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    // LOG_ERROR("stream connection close addr=[%p]", &m_stream_connection);
}

void stream_app::on_new_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    // LOG_ERROR("stream connection new conn addr=[%p]", &m_stream_connection);
}
