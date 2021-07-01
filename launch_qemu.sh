#!/usr/bin/env bash
FIRMWARE=$1

echo loading $FIRMWARE kernel into qemu

script qemu_output.log -c "exec timeout 5s \
    xpack-qemu-arm-2.8.0-11/bin/qemu-system-gnuarmeclipse \
    --verbose \
    --semihosting-config enable=on,target=native \
    -gdb tcp::3333 \
    -nographic -M STM32F4-Discovery \
    -no-reboot \
    -kernel $FIRMWARE "
