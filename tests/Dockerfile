FROM ubuntu:24.04

RUN apt-get update && apt-get install -y googletest gcc g++ wget make

RUN if [ "$(uname -m)" = "aarch64" ]; then \
wget https://github.com/Kitware/CMake/releases/download/v4.4.0/cmake-4.4.0-linux-aarch64.tar.gz  && tar -xvf cmake-4.4.0-linux-aarch64.tar.gz && cp -r cmake-4.4.0-linux-aarch64/* /usr; \
elif [ "$(uname -m)" = "x86_64" ]; then \
wget https://github.com/Kitware/CMake/releases/download/v4.4.0/cmake-4.4.0-linux-x86_64.tar.gz && tar -xvf cmake-4.4.0-linux-x86_64.tar.gz && cp -r cmake-4.4.0-linux-x86_64/* /usr; \
fi 

RUN cd /usr/src/googletest && cmake -S . && make && make install