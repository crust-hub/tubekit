# Tubekit

The C++ TCP server framework based on the Reactor model continues to implement POSIX thread pool, Epoll, non blocking IO, object pool, log, socket network programming, support the dynamic library to implement custom protocol extensions, and use http parser to process http requests. Currently only supports Linux systems

Platform: `Linux`  
Protocol: `HTTP` `TCP Stream(Protobuf)` `WebSocket`

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

## Directory Structure

[Directory Structure Link](./doc/dir_detail.md)

## Third Party

[@http-parser](https://github.com/nodejs/http-parser)  
[@lua](https://github.com/lua/lua)  
