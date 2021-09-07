FROM ubuntu

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -qy \
    g++ \
    liballegro4-dev \
    make \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /code
RUN find /usr/ -name liballeg.so.4.4 -exec cp {} . ';'
COPY Makefile Serpiente.cpp ./
RUN make
