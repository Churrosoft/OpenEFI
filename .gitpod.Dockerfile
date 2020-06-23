FROM jfloff/alpine-python:3.6-slim

RUN apk add --quiet --no-cache \
    autoconf \
    automake \
    binutils \
    make \
    file \
    git \
    gmp \
    isl \
    libc-dev \
    libtool \
    mpc1 \
    mpfr4 \
    musl-dev \
    pkgconf \
    zlib-dev \
    wget \
    build-base

RUN apk add  gcc-arm-none-eabi --update-cache --repository http://dl-3.alpinelinux.org/alpine/edge/testing/ --allow-untrusted
RUN python -V
RUN gcc -v
RUN arm-none-eabi-gcc --version
RUN arm-none-eabi-g++ --version

WORKDIR /src

CMD [ "make" ]