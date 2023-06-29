#!/bin/bash
echo "workdir: `pwd`"
cmake . && make && cd ./bin && ./tubekit
