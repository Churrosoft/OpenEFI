#!/bin/sh
cargo b
export PYTHONPATH=/usr/lib64/python3.8 && export PYTHONHOME=/usr/bin/python.3.8
arm-none-eabi-gdb -x ./debug.gdb ./target/thumbv7em-none-eabihf/release/open_efi
