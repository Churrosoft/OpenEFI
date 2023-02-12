mod handle_core;

use arrayvec::ArrayVec;
use cortex_m_semihosting::hprintln;
use usb_device::bus::{UsbBus, UsbBusAllocator};
use usb_device::device::{UsbDevice, UsbDeviceBuilder, UsbVidPid};
use crate::app::util;


#[repr(C, packed)]
#[derive(Debug, Copy, Clone)]
pub struct SerialMessage {
    pub protocol: u8,
    pub command: u8,
    pub status: u8,
    pub payload: [u8; 123],
    pub crc: u16
}

pub fn new_device<B>(bus: &UsbBusAllocator<B>) -> UsbDevice<'_, B> where B: UsbBus {
    UsbDeviceBuilder::new(bus, UsbVidPid(0x1209, 0xeef1))
        .manufacturer("Churrosoft")
        .product("OpenEFI | uEFI v2")
        .serial_number(util::get_serial_str())
        .device_release(0x0200)
        .self_powered(false)
        .max_power(250)
        .max_packet_size_0(64)
        .build()
}

pub fn process_command(buf: [u8; 128]) -> Option<SerialMessage> {
    let mut payload = [0u8; 123];
    payload.copy_from_slice(&buf[3..126]);

    let crc = ((buf[126] as u16) << 8) | buf[127] as u16;

    let serial_cmd = SerialMessage {
        protocol: buf[0],
        status: buf[1],
        command: buf[2],
        payload,
        crc,
    };

    hprintln!("CDC Message:\n - Proto {}\n - Commd {}\n - Statu {}\n - CRC:  {}", serial_cmd.protocol, serial_cmd.command, serial_cmd.status, crc);

    if serial_cmd.protocol != 1 {
        return None;
    }

    match serial_cmd.command & 0xF0 {
        0x00 => handle_core::handler(serial_cmd),
        _ => None  // TODO: Error message
    }
}

pub fn finish_message(message: SerialMessage) -> [u8; 128] {
    let mut message_buf = ArrayVec::<u8, 128>::new();
    message_buf.push(message.protocol);
    message_buf.push(message.status);
    message_buf.push(message.command);
    message_buf.try_extend_from_slice(&message.payload).unwrap();
    // Add empty CRC
    message_buf.push(0);
    message_buf.push(0);
    
    let payload: [u8; 126] = message_buf.take().into_inner().unwrap()[0..126].try_into().unwrap();
    let crc = util::crc16(&payload, 126);

    message_buf.clear();
    message_buf.try_extend_from_slice(&payload).unwrap();
    message_buf.try_extend_from_slice(&crc.to_be_bytes()).unwrap();

    message_buf.take().into_inner().unwrap()
}