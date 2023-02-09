vidpid_to_devs(){
        find $(grep -l "PRODUCT=$(printf "%x/%x" "0x${1%:*}" "0x${1#*:}")" \
                  /sys/bus/usb/devices/[0-9]*:*/uevent | sed 's,uevent$,,') \
           /dev/null -name dev -o -name dev_id |
        sed 's,[^/]*$,uevent,' |
        xargs sed -n -e s,DEVNAME=,/dev/,p -e s,INTERFACE=,,p
}
ttyPorts=$(vidpid_to_devs 1d50:6018)
ttyPortArr=($ttyPorts)

arm-none-eabi-gdb -nx --batch \
  -ex "target extended-remote ${ttyPortArr}" \
  -ex 'monitor swdp_scan' \
  -ex 'attach 1' \
  -ex 'load' \
  -ex 'compare-sections' \
  -ex 'kill' \
./target/thumbv7em-none-eabihf/debug/open_efi 
