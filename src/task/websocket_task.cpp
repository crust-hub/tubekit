// loading file sucess : / mnt / c / Users / gaowanlu / Desktop / MyProject / tubekit / bin / config / main.ini server start... -- -- -- -- -- -- -- --Accept : text / html, application / xhtml + xml, application / xml;
// q = 0.9, image / avif, image / webp, image / apng, * /*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Accept-Encoding:gzip, deflate
// Accept-Language:zh-CN,zh-TW;q=0.9,zh;q=0.8,en-US;q=0.7,en;q=0.6,und;q=0.5
// Cache-Control:max-age=0
// Connection:keep-alive
// Host:172.29.94.203:20023
// Upgrade-Insecure-Requests:1
// User-Agent:Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36
// ----------------
// http upgrade
// ----------------
// Accept-Encoding:gzip, deflate
// Accept-Language:zh-CN,zh-TW;q=0.9,zh;q=0.8,en-US;q=0.7,en;q=0.6,und;q=0.5
// Cache-Control:no-cache
// Connection:Upgrade
// Host:172.29.94.203:20023
// Origin:http://172.29.94.203:20023
// Pragma:no-cache
// Sec-WebSocket-Extensions:permessage-deflate; client_max_window_bits
// Sec-WebSocket-Key:2xREpot0G4dvmfquMHp8JQ==
// Sec-WebSocket-Version:13
// Upgrade:websocket
// User-Agent:Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36
// ----------------
// ----------------
// Accept:*/ *Accept - Encoding : gzip, deflate Accept - Language : zh - CN, zh - TW;
// q = 0.9, zh;
// q = 0.8, en - US;
// q = 0.7, en;
// q = 0.6, und;
// q = 0.5 Connection : keep - alive Host : 172.29.94.203 : 20023 User - Agent : Mozilla / 5.0(Windows NT 10.0; Win64; x64)AppleWebKit / 537.36(KHTML, like Gecko)Chrome / 119.0.0.0 Safari / 537.36 -- -- -- -- -- -- -- --

#include "task/websocket_task.h"

using namespace tubekit::task;
using namespace tubekit::socket;

websocket_task::websocket_task(tubekit::socket::socket *m_socket) : task(m_socket)
{
}

websocket_task::~websocket_task()
{
}

void websocket_task::run()
{
}

void websocket_task::destroy()
{
}