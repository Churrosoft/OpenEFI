use arrayvec::ArrayVec;
use rtic::Mutex;
use usb_device::{
    bus::{UsbBus, UsbBusAllocator},
    device::{UsbDevice, UsbDeviceBuilder, UsbVidPid},
};

use crate::{
    app,
    app::{logging, util},
};

mod handle_core;
pub mod handle_tables;
pub mod handle_pmic;
pub mod handle_engine;

#[repr(C, packed)]
#[derive(Debug, Copy, Clone)]
pub struct SerialMessage {
    pub protocol: u8,
    pub command: u8,
    pub status: u8,
    pub code: u8,
    pub payload: [u8; 122],
    pub crc: u16,
}

#[derive(Debug)]
#[repr(u8)]
pub enum SerialStatus {
    Error = 0b0000_0000,
    Ok = 0b0100_0000,
    UploadOk = 0x7c,
    DataChunk = 0x7d,
    DataChunkEnd = 0x7e,
}

#[repr(u8)]
pub enum SerialCode {
    None = 0x00,

    FWRequestBootloader = 0x01,
    FWRebootUnsafe = 0x03,

    ParseError = 0x5f,
    UnknownCmd = 0x7f,

    // Tables
    UnknownTable = 51,
    TableNotLoaded = 50,
    TableCrcError = 52,

    // PMIC:
    RequestFastStatus = 60,
    RequestIgnitionStatus = 61,
    RequestInjectionStatus = 62,
}

pub fn new_device<B>(bus: &UsbBusAllocator<B>) -> UsbDevice<'_, B>
    where
        B: UsbBus,
{
    UsbDeviceBuilder::new(bus, UsbVidPid(0x1209, 0xeef1))
        .manufacturer("Churrosoft")
        .product("OpenEFI | uEFI v3.4.x")
        .serial_number(util::get_serial_str())
        .device_release(0x0200)
        .self_powered(false)
        .max_power(250)
        .max_packet_size_0(64)
        .build()
}

pub fn process_command(buf: [u8; 128]) {
    let mut payload = [0u8; 122];
    payload.copy_from_slice(&buf[4..126]);

    let crc = ((buf[126] as u16) << 8) | (buf[127] as u16);

    let serial_cmd = SerialMessage {
        protocol: buf[0],
        status: buf[1],
        code: buf[2],
        command: buf[3],
        payload,
        crc,
    };

    logging::host::debug!(
        "CDC Message:\n - Proto {}\n - Commd {}\n - Statu {}\n - Code {}\n - CRC:  {}",
        serial_cmd.protocol,
        serial_cmd.command,
        serial_cmd.status,
        serial_cmd.code,
        crc
    );

    if serial_cmd.protocol != 1 {
        return;
    }

    match serial_cmd.command & 0xf0 {
        0x00 => handle_core::handler(serial_cmd),
        0x10 => app::table_cdc_callback::spawn(serial_cmd).unwrap(),
        0x20 => { /* TODO: injection */ }
        0x30 => { /* TODO: ignition */ }
        0x40 => { /* TODO: DTC */ }
        0x50 => { /* TODO: Dashboard / RealTime Data */ }
        0x60 => {
            /* TODO: General engine configuration */
            app::engine_cdc_callback::spawn(serial_cmd).unwrap()
        }
        0x70 => { /* TODO: Debug console */ }
        0x80 => app::pmic_cdc_callback::spawn(serial_cmd).unwrap(),
        0x90 => app::debug_demo::spawn(serial_cmd.command & 0b00001111).unwrap(),
        _ => {
            app::send_message::spawn(
                SerialStatus::Error,
                SerialCode::UnknownCmd as u8,
                serial_cmd,
            )
                .unwrap();
        }
    }
}

pub fn finish_message(message: SerialMessage) -> [u8; 128] {
    let mut message_buf = ArrayVec::<u8, 128>::new();
    message_buf.push(message.protocol);
    message_buf.push(message.status);
    message_buf.push(message.code);
    message_buf.push(message.command);
    message_buf.try_extend_from_slice(&message.payload).unwrap();
    // Add empty CRC
    message_buf.push(0);
    message_buf.push(0);

    let payload: [u8; 126] = message_buf.take().into_inner().unwrap()[0..126]
        .try_into()
        .unwrap();
    let crc = util::crc16(&payload, 126);

    message_buf.clear();
    message_buf.try_extend_from_slice(&payload).unwrap();
    message_buf
        .try_extend_from_slice(&crc.to_be_bytes())
        .unwrap();

    message_buf.take().into_inner().unwrap()
}

// Send a message via web serial.
pub(crate) fn send_message(
    mut ctx: app::send_message::Context,
    status: SerialStatus,
    code: u8,
    mut message: SerialMessage,
) {
    message.status = status as u8;
    message.code = code;

    ctx.shared.usb_cdc.lock(|cdc| {
        cdc.write(&finish_message(message)).unwrap();
    });
}
