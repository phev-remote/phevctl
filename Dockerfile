FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get -y install gcc cmake git
WORKDIR /phev/phevctl
COPY . .
RUN cmake --preset ci
RUN cmake --build --preset ci
CMD ["ctest", "--preset", "ci"]
