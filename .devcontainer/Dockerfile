FROM alpine:3.18
RUN apk add --no-cache git curl build-base gcc alpine-sdk eudev-dev libusb-dev libusb linux-headers bash libftdi1 libftdi1-dev

ENV RUSTUP_HOME=/rust
ENV CARGO_HOME=/cargo 
ENV PATH=/cargo/bin:/rust/bin:$PATH

RUN curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain nightly --no-modify-path

RUN rustup default nightly
RUN rustup target add thumbv7em-none-eabihf
RUN rustup component add llvm-tools-preview 

RUN cargo install cargo-binutils
RUN cargo install cargo-embed
RUN cargo install cargo-flash
RUN cargo install cargo-expand

WORKDIR /home/

COPY . .

RUN bash ./setup.sh

ENV PATH="/root/.cargo/bin:$PATH"