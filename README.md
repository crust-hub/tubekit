# Tubekit

The C++ TCP server framework based on the Reactor model continues to implement POSIX thread pool, Epoll, non blocking IO, object pool, log, socket network programming, support the dynamic library to implement custom protocol extensions, and use http parser to process http requests. Currently only supports Linux systems

Platform: `Linux`  
Protocol: `HTTP` `TCP Stream(Protobuf)` `WebSocket`  
Support TLS/SSL: `OpenSSL`  

## Get Start

prepare

```bash
$ sudo apt update
$ sudo apt install protobuf-compiler libprotobuf-dev
$ apt install g++ cmake make
$ git clone https://github.com/crust-hub/tubekit.git
```

Build

```bash
$ cd tubekit
$ cd protocol
$ make
$ cd ..
$ cmake .
$ make -j3
```

Config

```bash
$ vim bin/config/main.ini
```

Run

```bash
$ chmod +x ./run.sh
$ ./run.sh
```

Stop

```bash
$ chmod +x ./kill.sh
$ ./kill.sh
```

## App

support tcp keep-alive stream (protobuf) and http app (http-parser)、websocket

## Example

1. [framework config](https://github.com/crust-hub/tubekit/blob/main/bin/config/main.ini)
2. [stream protobuf app](https://github.com/crust-hub/tubekit/blob/main/src/app/stream_app.cpp)
3. [http app](https://github.com/crust-hub/tubekit/blob/main/src/app/http_app.cpp)
4. [websocket app](https://github.com/crust-hub/tubekit/blob/main/src/app/websocket_app.cpp)

## QPS

Intel(R) Xeon(R) CPU E5-2673 v4 @ 2.3GHz

`config/main.ini` theads:6

```bash
Concurrency  QPS            HttpRequest

10          5998.75/sec    10000
10          5829.98/sec    100000
10          3672.92/sec    500000

50          3970.39/sec    10000
50          4854.57/sec    100000
50          5002.02/sec    500000

100         7370.73/sec    10000
100         7417.3/sec     100000
100         6780.85/sec    500000

500         7460.77/sec    10000
500         7717.21/sec    100000
500         7447.16/sec    500000

1000        7962.60/sec    10000
1000        7357.94/sec    100000
1000        7508.63/sec    500000

5000        4287.76/sec    10000
5000        5861.78/sec    100000
5000        7196.74/sec    500000
```

## Directory Structure

[Directory Structure Link](./doc/dir_detail.md)

## Third Party

[@http-parser](https://github.com/nodejs/http-parser)  
[@lua](https://github.com/lua/lua)  
