use crate::app::webserial::SerialMessage;

pub fn handler(command: SerialMessage) -> Option<SerialMessage> {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: 1,
        status: 0,
        payload: [0u8; 123],
        crc: 0,
    };
    
    match command.command {
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

            // TODO set status bits

            Some(response_buf)
        }
        _ => None // TODO command not found
    }
    
    
}