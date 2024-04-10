# Tubekit

Network Message Framework For Linux C++.

CPP: `c++17`  
Platform: `linux`  
Protocol: `http` `tcp stream(protobuf)` `websocket`  
Support TLS/SSL: `openssl`  
Script: `lua`  

## Get Start

If there are already dependencies to be installed on the host, please selectively ignore them.

### Ubuntu (Docker)

```bash
$ docker run -it ubuntu
$ sudo apt-get update
$ sudo apt-get install apt-utils -y
$ sudo apt-get install cmake g++ make git -y
$ sudo apt-get install protobuf-compiler libprotobuf-dev -y
$ sudo apt-get install libssl-dev -y
$ git clone https://github.com/crust-hub/tubekit.git
$ cd tubekit
$ cd protocol
$ make
$ cd ..
$ cmake .
$ make -j4
```

### CentOS8 (Docker)

[centos8](./Centos8.md)

### Config

```bash
$ sudo mkdir /tubekit_static
$ vim bin/config/main.ini
```

### Tubekit Start

```bash
$ cd bin
$ ./tubekit
$ ps -ef | grep tubekit
```

## Docker Example

```bash
$ docker run -it --privileged -p 20023:20023 -v ${LOCAL_HTTP_DIR_PATH}:/tubekit_static gaowanlu/tubekit:latest bash
$ cd ./bin
$ ./tubekit
```

## APP Example

support tcp keep-alive stream (protobuf) and http app (http-parser)、websocket

1. [framework config](https://github.com/crust-hub/tubekit/blob/main/bin/config/main.ini)
2. [stream protobuf app](https://github.com/crust-hub/tubekit/blob/main/src/app/stream_app.cpp)
3. [http app](https://github.com/crust-hub/tubekit/blob/main/src/app/http_app.cpp)
4. [websocket app](https://github.com/crust-hub/tubekit/blob/main/src/app/websocket_app.cpp)

## Requests Per Second

CPU: Intel(R) Core(TM) i5-9600KF CPU @ 3.70 GHz   
OS : WSL2 Ubuntu Mem 8GB  (Windows 11)

```ini
config/main.ini 
    theads:6  
    max_conn:10000  
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

10          14051.34/sec         3004.74 /sec                        10000           1072bytes
10          13594.83/sec         3456.10 /sec                        100000          1072bytes
10          13208.49/sec         3519.01 /sec                        500000          1072bytes

50          14080.68/sec         3860.68 /sec                        10000           1072bytes
50          14643.93/sec         3852.66 /sec                        100000          1072bytes
50          14618.96/sec         3827.33 /sec                        500000          1072bytes

100         13570.44/sec         3902.76 /sec                        10000           1072bytes
100         14651.17/sec         3940.42 /sec                        100000          1072bytes
100         14592.67/sec         3811.21 /sec                        500000          1072bytes

500         13165.41/sec         3863.30 /sec                        10000           1072bytes
500         14612.57/sec         3896.56 /sec                        100000          1072bytes
500         14491.29/sec         3911.07 /sec                        500000          1072bytes

1000        14195.35/sec         3271.67 /sec                        10000           1072bytes
1000        14648.93/sec         3685.15 /sec                        100000          1072bytes
1000        14601.71/sec         3879.07 /sec                        500000          1072bytes
1000        14655.50/sec         3666.36 /sec                        1000000         1072bytes
```

## Third-Party

1、[@http-parser](https://github.com/nodejs/http-parser)  2、[@lua](https://github.com/lua/lua)  
3、[@protobuffer](https://github.com/protocolbuffers/protobuf)  4、[@openssl](https://github.com/openssl/openssl)  
