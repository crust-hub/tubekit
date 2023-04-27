# Tubekit

The C++ TCP server framework based on the Reactor model continues to implement POSIX thread pool, Epoll, non blocking IO, object pool, log, socket network programming, support the dynamic library to implement custom protocol extensions, and use http parser to process http requests. Currently only supports Linux systems

## Get Start

prepare

```bash
$ apt install g++ cmake make
$ git clone https://github.com/crust-hub/tubekit.git
$ cd tubekit/external
$ make
$ make install
$ cd ..
$ cmake .
$ make
```

Run

```bash
sudo bash ./run.sh
```

Stop

```bash
sudo bash ./kill.sh
```

## Directory Structure

[Directory Structure Link](./doc/dir_detail.md)

## Third Party

[@http-parser](https://github.com/nodejs/http-parser)  
[@yazi](https://github.com/oldjun/yazi)(source of inspiration)
