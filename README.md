# tubekit

web service framework for C++ ，rebuild from the yazi framework.

## start

prepare  

```bash
g++ cmake make http-parser
```

run  

```bash
sudo bash ./run.sh
```

stop  

```bash
sudo bash ./kill.sh
```

## docker image

```bash
docker pull gaowanlu/tubekit:latest
docker run gaowanlu/tubekit
```

# third party

[@http-parser](https://github.com/nodejs/http-parser)  
[@yazi](https://github.com/oldjun/yazi)(source of inspiration)  

## to_do

* json parser
