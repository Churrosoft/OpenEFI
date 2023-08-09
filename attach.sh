#!/bin/sh
cargo b
arm-none-eabi-gdb -x ./debug.gdb ./target/thumbv7em-none-eabihf/release/open_efi
