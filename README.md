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
$ docker run -it -p 20023:20023 -v ${LOCAL_HTTP_DIR_PATH}:/tubekit_static gaowanlu/tubekit:latest bash
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

## Third-Party

1、[@http-parser](https://github.com/nodejs/http-parser)  2、[@lua](https://github.com/lua/lua)  
3、[@protobuffer](https://github.com/protocolbuffers/protobuf)  4、[@openssl](https://github.com/openssl/openssl)  

