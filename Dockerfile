FROM ubuntu:12.04
MAINTAINER Iwan Karamazow

RUN apt-get update
RUN apt-get install -y git wget xz-utils g++

ENV NODE_VERSION 4.0.0

RUN wget https://nodejs.org/dist/v$NODE_VERSION/node-v$NODE_VERSION-linux-x64.tar.xz \
  && tar -xJf "node-v$NODE_VERSION-linux-x64.tar.xz" -C /usr/local --strip-components=1 \
  && rm "node-v$NODE_VERSION-linux-x64.tar.xz"

RUN mkdir -p /out

COPY . /out

WORKDIR /out

RUN npm run build 

CMD ["/bin/bash"]
