// g++ -o client.exe client.cpp ../protocol/proto_res/*.pb.cc --std=c++17 -lprotobuf -lpthread
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
#include <thread>
#include <chrono>

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

    // const char *server_ip = "61.171.51.135";
    const char *server_ip = "127.0.0.1";
    int server_port = 20023;

    int n;
    bool stop_flag = false;

    constexpr int thread_count = 20;

    for (int loop = 0; loop < thread_count; loop++)
    {
        std::thread m_thread(
            [server_port, server_ip, argv, loop, &stop_flag]()
            {
                ProtoPackage message;
                ProtoCSReqExample exampleReq;
                std::string send_str(argv[1]);
                exampleReq.set_testcontext(send_str);
                message.set_cmd(ProtoCmd::CS_REQ_EXAMPLE);

                uint64_t send_size = 0;
                uint64_t recv_size = 0;
                uint64_t last_print_size = 0;

                constexpr int client_cnt = 300;
                int client_socket_arr[client_cnt]{0};
                for (int client_idx = 0; client_idx < client_cnt; client_idx++)
                {
                    int &client_socket = client_socket_arr[client_idx];
                    client_socket = socket(AF_INET, SOCK_STREAM, 0);
                    if (client_socket == -1)
                    {
                        perror("Socket creation failed");
                        return;
                    }

                    sockaddr_in server_address;
                    server_address.sin_family = AF_INET;
                    server_address.sin_port = htons(server_port);
                    server_address.sin_addr.s_addr = inet_addr(server_ip);

                    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
                    {
                        perror("Connection failed");
                        close(client_socket);
                        return;
                    }
                }

                while (true)
                {
                    if (stop_flag)
                    {
                        for (int i = 0; i < client_cnt; i++)
                        {
                            if (client_socket_arr[i] > 0)
                            {
                                close(client_socket_arr[i]);
                            }
                        }
                        return;
                    }
                    for (int client_idx = 0; client_idx < client_cnt; client_idx++)
                    {
                        int &client_socket = client_socket_arr[client_idx];
                        if (client_socket <= 0)
                        {
                            continue;
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
                                // perror("Send Head failed");
                                close(client_socket);
                                client_socket = 0;
                                return;
                            }
                            else
                            {
                                send_size++;
                                // printf("Send Head succ\n");
                            }
                            // printf("Send all bytes package %ld body %ld all %ld\n", data.size(), body_str.size(), data.size() + body_str.size());

                            // block read res package
                            char buffer[1024000]{0};
                            uint32_t data_len = 0;

                            while (true)
                            {
                                int recv_len = read(client_socket, buffer + data_len, 1024000 - data_len);
                                if (recv_len == 0)
                                {
                                    close(client_socket);
                                    client_socket = 0;
                                    break;
                                }
                                if (recv_len == -1)
                                {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                                    continue;
                                }

                                data_len += recv_len;

                                ProtoPackage protoPackage;
                                if (!protoPackage.ParseFromArray(buffer, data_len))
                                {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                                    continue;
                                }
                                else
                                {
                                    if (protoPackage.cmd() == ProtoCmd::CS_RES_EXAMPLE)
                                    {
                                        ProtoCSResExample exampleRes;
                                        if (exampleRes.ParseFromString(protoPackage.body()))
                                        {
                                            recv_size++;
                                            if ((recv_size - last_print_size) > 1000)
                                            {
                                                printf("RES(thread[%d] send_size[%lu],recv_size[%lu])=>%s\n", loop, send_size, recv_size, exampleRes.testcontext().c_str());
                                                last_print_size = recv_size;
                                            }
                                            break;
                                        }
                                        else
                                        {
                                            std::cout << "exampleRes.ParseFromString(protoPackage.body()) failed" << std::endl;
                                            close(client_socket);
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        std::cout << "res package cmd is not CS_RES_EXAMPLE" << std::endl;
                                        close(client_socket);
                                        client_socket = 0;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            });
        m_thread.detach();
    }

    std::cin >> n;
    stop_flag = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    return 0;
}
