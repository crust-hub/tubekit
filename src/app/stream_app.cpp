#include "app/stream_app.h"
#include "proto_res/proto_cmd.pb.h"
#include "proto_res/proto_example.pb.h"
#include "proto_res/proto_message_head.pb.h"

using tubekit::app::stream_app;
using tubekit::connection::stream_connection;

void stream_app::process_connection(tubekit::connection::stream_connection &m_stream_connection)
{
    ProtoMessageHead protoMessageHead;
    protoMessageHead.set_cmd(0);
    protoMessageHead.set_bodylen(0);
    ProtoExampleReq protoExampleReq;
    protoExampleReq.set_testcontext("hello world");
    m_stream_connection.m_recv_buffer.clear();
    m_stream_connection.send("hello tubekit", 14);
}
