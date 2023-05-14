FROM ubuntu:latest
RUN mkdir -p /tubekit
COPY . /tubekit
WORKDIR /tubekit
RUN apt-get update && apt-get install -y apt-utils
RUN apt-get install cmake g++ make -y
RUN apt-get install git
WORKDIR /tubekit
CMD rm -rf CMakeCache.txt \
    &&cmake . \
    && make -j2 \
    && cd bin \
    && ls \
