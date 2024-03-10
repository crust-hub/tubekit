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
$ docker run -it -p 20023:20023 -v ${LOCAL_HTTP_DIR_PATH}:/tubekit_static gaowanlu/tubekit:latest bash
$ cd ./bin
$ ./tubekit
```

## App Example

support tcp keep-alive stream (protobuf) and http app (http-parser)、websocket

1. [framework config](https://github.com/crust-hub/tubekit/blob/main/bin/config/main.ini)
2. [stream protobuf app](https://github.com/crust-hub/tubekit/blob/main/src/app/stream_app.cpp)
3. [http app](https://github.com/crust-hub/tubekit/blob/main/src/app/http_app.cpp)
4. [websocket app](https://github.com/crust-hub/tubekit/blob/main/src/app/websocket_app.cpp)

## Requests Per Second

CPU: Intel(R) Core(TM) i7-9750H CPU @ 2.60 GHz  
Mem: 8GB  
OS : WSL2 Ubuntu (Windows 11)

```ini
config/main.ini 
    theads:8  
    max_conn:2000  
```

apache2-utils testing

```bash
# tubekit
$ ab -c {{concurrency}} -n {{httpRequest}} http://IP:20023/
# nodejs
$ ab -c {{concurrency}} -n {{httpRequest}} http://IP:20025/
```

```bash
# apache2-utils ab report
concurrency bin/tubekit          test/node_http_server.js            httpRequest     responseBodySize

10          11959.51/sec         3636.00 /sec                        10000           890bytes
10          7897.85 /sec         3244.76 /sec                        100000          890bytes
10          8050.12 /sec         3047.05 /sec                        500000          890bytes

50          9571.97 /sec         3484.50 /sec                        10000           890bytes
50          7679.47 /sec         3453.35 /sec                        100000          890bytes
50          7744.02 /sec         3389.12 /sec                        500000          890bytes

100         10784.61/sec         3819.69 /sec                        10000           890bytes
100         7715.26 /sec         3447.19 /sec                        100000          890bytes
100         7361.67 /sec         3566.64 /sec                        500000          890bytes

500         7744.84 /sec         4101.63 /sec                        10000           890bytes
500         7813.42 /sec         3593.05 /sec                        100000          890bytes
500         7663.37 /sec         3654.70 /sec                        500000          890bytes

1000        10670.04/sec         3381.92 /sec                        10000           890bytes
1000        7682.88 /sec         3459.71 /sec                        100000          890bytes
1000        7754.77 /sec         3667.54 /sec                        500000          890bytes
1000        7519.90 /sec         3691.43 /sec                        1000000         890bytes
```

## Directory Structure

[Directory Structure Link](./doc/dir_detail.md)

## Third Party

[@http-parser](https://github.com/nodejs/http-parser)  
[@lua](https://github.com/lua/lua)  
