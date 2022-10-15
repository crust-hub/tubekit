import socket
import struct
import time

def send_data(client, data):
    cmd = 1
    data_len = len(data)
    data = struct.pack(f'8sII{data_len}s', b'work', cmd, data_len, data.encode('utf-8'))
    client.send(data)  # 发送TCP数据
    info = client.recv(1024).decode()
    print(len(info))


if __name__ == '__main__':
    data=''
    for i in range(10):
        data=data+'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabhdsbjchbfvjbdfjvbfjdhvbjhfbh'
    index = 0 

    host = '61.171.51.135'
    port = 2022
    start_time = time.time()
    for index in range(10000):
        client = socket.socket()  # 创建TCP/IP套接字
        client.connect((host, port))  # 主动初始化TCP服务器连接
        print('连接成功')
        send_data(client, data)
        client.close()
        print(index)
        index+=1
    end_time=time.time()
    print('time count: ')
    print((end_time-start_time))
