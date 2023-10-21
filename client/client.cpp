// g++ -o client.exe client.cpp ../protocol/proto_res/*.pb.cc -lprotobuf
#include <iostream>
#include <string>
#include <sstream>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../protocol/proto_res/proto_cmd.pb.h" // 导入你的 Protocol Buffers 头文件
#include "../protocol/proto_res/proto_example.pb.h"
#include "../protocol/proto_res/proto_message_head.pb.h"

int main()
{
    // 创建 Protocol Buffers 消息对象并填充数据
    ProtoMessageHead message;
    ProtoExampleReq exampleReq;
    std::string send_str(1324 * 10, 'a');
    exampleReq.set_testcontext(send_str);
    message.set_cmd(1000);

    // 目标服务器的 IP 和端口
    const char *server_ip = "172.29.94.203";
    int server_port = 20023;

    // 创建套接字
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);

    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    // 序列化 Protocol Buffers 消息为字节流
    std::string body_str;
    body_str.resize(exampleReq.ByteSizeLong());
    google::protobuf::io::ArrayOutputStream output_stream_1(&body_str[0], body_str.size());
    google::protobuf::io::CodedOutputStream coded_output_1(&output_stream_1);
    exampleReq.SerializeToCodedStream(&coded_output_1);

    message.set_bodylen(body_str.size());
    std::string data;
    data.resize(message.ByteSizeLong());
    message.set_headlen(message.ByteSizeLong());
    google::protobuf::io::ArrayOutputStream output_stream(&data[0], data.size());
    google::protobuf::io::CodedOutputStream coded_output(&output_stream);
    message.SerializeToCodedStream(&coded_output);

    // 发送协议头数据到服务器
    if (send(client_socket, data.c_str(), data.size(), 0) != data.size())
    {
        perror("Send Head failed");
        close(client_socket);
        return 1;
    }
    else
    {
        printf("Send Head succ\n");
    }

    // 发送协议包
    if (send(client_socket, body_str.c_str(), body_str.size(), 0) != body_str.size())
    {
        perror("Send Body failed");
        close(client_socket);
        return 1;
    }
    else
    {
        printf("Send Body succ\n");
    }

    // 在这里可以添加接收服务器的响应（如果有的话
    printf("Send all bytes head %ld body %ld all %ld\n", data.size(), body_str.size(), data.size() + body_str.size());

    // sleep(10);
    //  关闭套接字
    close(client_socket);

    return 0;
}
