FROM ubuntu:latest
RUN mkdir -p /tubekit
COPY . /tubekit
WORKDIR /tubekit
RUN apt-get update && apt-get install -y apt-utils
RUN apt-get install cmake g++ make git -y
RUN apt-get install protobuf-compiler libprotobuf-dev  -y
RUN apt-get install libssl-dev -y
WORKDIR /tubekit
CMD rm -rf CMakeCache.txt \
    && cd protocol \
    && cd .. \
    && cmake . \
    && make -j3 \
    && cd bin \
    && ls \
