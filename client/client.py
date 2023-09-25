import socket
import struct
import time
import threading

m_socket_list = []
will_send_data = ''
# host = '61.171.51.135'
host = '172.29.94.203'
port = 20023
exit_flag = False  # 退出标志

def recv_thread():
    all_recv_data_bytes = 0
    while not exit_flag:
        for client in m_socket_list:
            try:
                data = client.recv(1024)
                all_recv_data_bytes += len(data)
            except BlockingIOError:
                pass
            except Exception as e:
                print("Error:", e)
                break
        print("all_recv_data_bytes:",all_recv_data_bytes)

if __name__ == '__main__':
    for i in range(10):
        will_send_data = will_send_data + 'aaaaaaaaaaaaaaaaaaaa高万禄aaaaaaaaaaaaaaaabhdsbjchbfvjbdfjvbfjdhvbjhfbh'
    for i in range(1000):
        client = socket.socket()  # tcp socket
        client.connect((host, port))  # init connection
        client.setblocking(False)
        m_socket_list.append(client)
    print('connected')

    # 创建接收线程
    thread1 = threading.Thread(target=recv_thread, args=())
    thread1.start()

    # 主线程发送数据
    all_send_data_bytes = 0
    try:
        while True:
            for client in m_socket_list:
                try:
                    sent = client.send(will_send_data.encode())
                    all_send_data_bytes = all_send_data_bytes + sent
                except BlockingIOError:
                    pass
                except Exception as e:
                    print("Error:", e)
                    break
            print("all_send_data_bytes:",all_send_data_bytes)
    except KeyboardInterrupt:
        # 用户按下 Ctrl+C 退出程序
        exit_flag = True

    # 主线程退出后，等待接收线程完成
    thread1.join()

    # 关闭所有套接字
    for client in m_socket_list:
        client.close()

    print("All threads have finished.")
