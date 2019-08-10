FROM ubuntu:disco
RUN apt-get -y update && apt-get upgrade -y && apt-get -y install build-essential cmake git
WORKDIR /src
RUN git clone https://github.com/papawattu/msg-core && git clone https://github.com/papawattu/phev-core.git && git clone https://github.com/DaveGamble/cJSON.git
RUN cd /src/cJSON && mkdir build && cd build && cmake .. && make && make install
RUN cd /src/msg-core && mkdir build && cd build && cmake .. && make && make install
RUN cd /src/phev-core && mkdir build && cd build && cmake .. && make && make install
COPY . /src/adapter
WORKDIR /src/adapter
RUN mkdir -p build && cd build && cmake .. && make 

