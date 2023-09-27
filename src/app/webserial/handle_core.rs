use rtic_sync::channel::Sender;
use crate::{
    app,
    app::webserial::{SerialCode, SerialMessage, SerialStatus},
};
pub fn handler(command: SerialMessage, mut webserial_sender: &mut Sender<SerialMessage, 30>) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: command.command,
        status: 0,
        code: 0,
        payload: [0u8; 122],
        crc: 0,
    };

    match command.command & 0b00001111 {
        0x01 => {
            response_buf.payload[0] = 1; // BOARD TYPE

            let efi_ver_major: u16 = env!("CARGO_PKG_VERSION_MAJOR").parse().unwrap();
            let efi_ver_minor: u16 = env!("CARGO_PKG_VERSION_MINOR").parse().unwrap();
            let efi_ver_patch: u16 = env!("CARGO_PKG_VERSION_PATCH").parse().unwrap();

            response_buf.payload[1] = ((efi_ver_major >> 8) & 0xFF) as u8;
            response_buf.payload[2] = (efi_ver_major & 0xFF) as u8;

            response_buf.payload[3] = ((efi_ver_minor >> 8) & 0xFF) as u8;
            response_buf.payload[4] = (efi_ver_minor & 0xFF) as u8;

            response_buf.payload[5] = ((efi_ver_patch >> 8) & 0xFF) as u8;
            response_buf.payload[6] = (efi_ver_patch & 0xFF) as u8;

            response_buf.status = SerialStatus::Ok as u8;
            response_buf.code = 0;
            webserial_sender.try_send(response_buf).ok();
            // app::send_message::spawn(SerialStatus::Ok,0,response_buf).unwrap();
        }
        _ => {
            response_buf.status = SerialStatus::Error as u8;
            response_buf.code = SerialCode::UnknownCmd as u8;
            // webserial_sender.try_send(response_buf).ok();
            app::send_message::spawn(SerialStatus::Error,SerialCode::UnknownCmd as u8 ,response_buf).unwrap();
        }
    };
}
