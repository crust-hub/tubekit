FROM ubuntu:latest
RUN mkdir -p /tubekit
COPY . /tubekit
WORKDIR /tubekit
RUN sed -i "s@http://.*security.ubuntu.com@https://mirrors.tuna.tsinghua.edu.cn@g" /etc/apt/sources.list
RUN apt update -y
RUN apt install cmake g++ make -y
CMD rm -rf CMakeCache.txt \
    &&cmake . \
    && make -j8 \
    && cd bin \
    && ./tubekit \
