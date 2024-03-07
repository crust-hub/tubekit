# Tubekit

The C++ TCP server framework based on the Reactor model continues to implement POSIX thread pool, Epoll, non blocking IO, object pool, log, socket network programming, support the dynamic library to implement custom protocol extensions, and use http parser to process http requests. Currently only supports Linux systems

Platform: `Linux`  
Protocol: `HTTP` `TCP Stream(Protobuf)` `WebSocket`  
Support TLS/SSL: `OpenSSL`  
Script: `Lua`  

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

## Docker

```bash
docker run -it -p 20023:20023 -v ${LOCAL_HTTP_DIR_PATH}:/tubekit_static gaowanlu/tubekit:latest bash
$ cd protocol
$ cd ..
$ cmake .
$ make -j3
$ cd ./bin
$ ./tubekit
```

## App

support tcp keep-alive stream (protobuf) and http app (http-parser)、websocket

## Example

1. [framework config](https://github.com/crust-hub/tubekit/blob/main/bin/config/main.ini)
2. [stream protobuf app](https://github.com/crust-hub/tubekit/blob/main/src/app/stream_app.cpp)
3. [http app](https://github.com/crust-hub/tubekit/blob/main/src/app/http_app.cpp)
4. [websocket app](https://github.com/crust-hub/tubekit/blob/main/src/app/websocket_app.cpp)

## QPS

CPU: Intel(R) Core(TM) i7-9750H CPU @ 2.60 GHz  
Mem: 8GB  
OS : WSL2 Ubuntu (Windows 11)

```ini
config/main.ini 
    theads:6  
    max_conn:2000  
```

testing result

```bash
$ ab -c {{Concurrency}} -n {{HttpRequest}} http://IP:20023/
```

```bash
Concurrency QPS             HttpRequest

10          13872.80/sec    10000
10          13147.52/sec    100000
10          9789.71/sec     500000

50          13540.06/sec    10000
50          11490.66/sec    100000
50          9470.44/sec     500000

100         14059.34/sec    10000
100         11530.81/sec    100000
100         9420.90/sec     500000

500         13839.18/sec    10000
500         12407.10/sec    100000
500         9438.90/sec     500000

1000        13732.30/sec    10000
1000        11226.25/sec    100000
1000        9641.19/sec     500000
```

## Directory Structure

[Directory Structure Link](./doc/dir_detail.md)

## Third Party

[@http-parser](https://github.com/nodejs/http-parser)  
[@lua](https://github.com/lua/lua)  
