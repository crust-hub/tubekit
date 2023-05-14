FROM ubuntu:latest
RUN mkdir -p /tubekit
COPY . /tubekit
WORKDIR /tubekit
RUN apt update -y
RUN apt install cmake g++ make -y
RUN apt install git
WORKDIR /tubekit
CMD rm -rf CMakeCache.txt \
    &&cmake . \
    && make -j2 \
    && cd bin \
    && ls \
