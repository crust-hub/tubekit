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
    accept_per_tick: 50  
```

apache2-utils testing, tubekit http and node test/node_http_server.js .

```bash
# tubekit
$ ab -c {{concurrency}} -n {{httpRequest}} http://IP:20023/node_http_server.js
# nodejs
$ ab -c {{concurrency}} -n {{httpRequest}} http://IP:20025/node_http_server.js
```

```bash
# apache2-utils ab report
concurrency ./bin/tubekit        node node_http_server.js            httpRequest     responseBodySize

10          12411.18/sec         3029.43 /sec                        10000           1072bytes
10          15181.95/sec         3696.97 /sec                        100000          1072bytes
10          15541.82/sec         3745.75 /sec                        500000          1072bytes

50          18585.67/sec         3967.86 /sec                        10000           1072bytes
50          20278.76/sec         4018.54 /sec                        100000          1072bytes
50          20674.26/sec         3987.77 /sec                        500000          1072bytes

100         18804.20/sec         3974.26 /sec                        10000           1072bytes
100         20632.04/sec         4075.96 /sec                        100000          1072bytes
100         20940.76/sec         4022.14 /sec                        500000          1072bytes

500         18832.39/sec         3854.39 /sec                        10000           1072bytes
500         20746.29/sec         4093.87 /sec                        100000          1072bytes
500         20914.47/sec         4031.80 /sec                        500000          1072bytes

1000        18302.92/sec         3326.02 /sec                        10000           1072bytes
1000        20758.29/sec         3785.80 /sec                        100000          1072bytes
1000        20956.58/sec         3993.80 /sec                        500000          1072bytes
1000        20963.26/sec         3826.19 /sec                        1000000         1072bytes

1000                             7066.19 /sec                        1000000         30  bytes
1000        38468.68/sec                                             1000000         1177bytes
```

## Third-Party

1、[@http-parser](https://github.com/nodejs/http-parser)  2、[@lua](https://github.com/lua/lua)  
3、[@protobuffer](https://github.com/protocolbuffers/protobuf)  4、[@openssl](https://github.com/openssl/openssl)  
5、[@zlib](https://github.com/madler/zlib)  

