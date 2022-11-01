FROM ubuntu:latest
RUN mkdir -p /tubekit
COPY . /tubekit
WORKDIR /tubekit
#RUN sed -i "s@http://.*security.ubuntu.com@https://mirrors.tuna.tsinghua.edu.cn@g" /etc/apt/sources.list
UN apt-get update && apt-get install -y --no-install-recommends apt-utils
RUN apt update -y
RUN apt install cmake g++ make -y
RUN apt install git
RUN git clone https://github.com/nodejs/http-parser.git
WORKDIR /tubekit/http-parser
RUN make
RUN make install
WORKDIR /tubekit
CMD rm -rf CMakeCache.txt \
    &&cmake . \
    && make -j8 \
    && cd bin \
    && ./tubekit \
