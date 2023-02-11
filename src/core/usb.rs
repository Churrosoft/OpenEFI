use usb_device::bus::{UsbBus, UsbBusAllocator};
use usb_device::device::{UsbDevice, UsbDeviceBuilder, UsbVidPid};
use crate::core::util::get_serial_str;

pub fn new_device<B>(bus: &UsbBusAllocator<B>) -> UsbDevice<'_, B> where B: UsbBus {
    UsbDeviceBuilder::new(bus, UsbVidPid(0x1209, 0xeef1))
        .manufacturer("Churrosoft")
        .product("OpenEFI | uEFI v2")
        .serial_number(get_serial_str())
        .device_release(0x0200)
        .self_powered(false)
        .max_power(250)
        .max_packet_size_0(64)
        .build()
}