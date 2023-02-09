arm-none-eabi-gdb -nx --batch \
  -ex 'target extended-remote /dev/ttyACM0' \
  -ex 'monitor swdp_scan' \
  -ex 'attach 1' \
  -ex 'load' \
  -ex 'compare-sections' \
  -ex 'kill' \
./target/thumbv7em-none-eabihf/debug/open_efi 
