FROM ubuntu:latest
RUN apt-get update && apt-get install build-essential cmake git
WORKDIR /src
RUN git clone https://github.com/papawattu/msg-core
RUN mkdir build && cd build && cmake .. && make && make install