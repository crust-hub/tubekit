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
#include <endian.h>

#include "../protocol/proto_res/proto_cmd.pb.h"
#include "../protocol/proto_res/proto_example.pb.h"
#include "../protocol/proto_res/proto_message_head.pb.h"

int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        std::cout << "arg<2" << std::endl;
        return 1;
    }
    ProtoPackage message;
    ProtoExampleReq exampleReq;
    std::string send_str(argv[1]);
    exampleReq.set_testcontext(send_str);
    message.set_cmd(ProtoCmd::EXAMPLE_REQ);

    //const char *server_ip = "61.171.51.135";
    const char *server_ip = "172.29.94.203";
    int server_port = 20023;

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

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    std::string body_str;
    body_str.resize(exampleReq.ByteSizeLong());
    google::protobuf::io::ArrayOutputStream output_stream_1(&body_str[0], body_str.size());
    google::protobuf::io::CodedOutputStream coded_output_1(&output_stream_1);
    exampleReq.SerializeToCodedStream(&coded_output_1);
    message.set_body(body_str);
    std::string data = message.SerializePartialAsString();

    uint64_t packageLen = data.size();

    for (int i = 0; i < 5; i++)
    {
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

        printf("Send all bytes package %ld body %ld all %ld\n", data.size(), body_str.size(), data.size() + body_str.size());
    }

    close(client_socket);

    return 0;
}
