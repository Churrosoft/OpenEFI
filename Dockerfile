FROM ubuntu:focal

ENV TZ=Europe/Kiev
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update \
    && apt-get install -y \
    software-properties-common \
    wget \
    make \
    git \
    gcc \
    g++

RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
RUN tar xjf ./gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 -C /usr/share/
RUN ln -s /usr/share/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc
RUN ln -s /usr/share/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
RUN ln -s /usr/share/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
RUN ln -s /usr/share/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-ld /usr/bin/arm-none-eabi-ld
RUN ln -s /usr/share/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-as /usr/bin/arm-none-eabi-as
RUN ln -s /usr/share/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-ar /usr/bin/arm-none-eabi-ar
RUN ln -s /usr/share/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-objcopy  /usr/bin/arm-none-eabi-objcopy
RUN ln -s /usr/lib/x86_64-linux-gnu/libncurses.so.6 /usr/lib/x86_64-linux-gnu/libncurses.so.5
RUN ln -s /usr/lib/x86_64-linux-gnu/libtinfo.so.6 /usr/lib/x86_64-linux-gnu/libtinfo.so.5
RUN arm-none-eabi-gcc --version
RUN arm-none-eabi-g++ --version

RUN gcc -v
RUN arm-none-eabi-gcc --version
RUN arm-none-eabi-g++ --version

WORKDIR /src

CMD [ "make" ]