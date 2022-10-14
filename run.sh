echo "workdir: $(pwd)"
cmake . && make && cd bin && nohup ./tubekit &
