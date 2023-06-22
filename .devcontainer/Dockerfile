FROM alpine:3.14
RUN apk add --no-cache git curl build-base gcc alpine-sdk eudev-dev libusb-dev libusb linux-headers bash curl zsh libressl-dev libftdi1-dev

ENV RUSTUP_HOME=/rust
ENV CARGO_HOME=/cargo 
ENV PATH=/cargo/bin:/rust/bin:$PATH

WORKDIR /home/

COPY . .

RUN bash ./setup.sh

ENV PATH="/root/.cargo/bin:$PATH"