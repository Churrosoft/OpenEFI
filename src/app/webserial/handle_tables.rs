use crate::app::memory::tables::{FlashT, Tables};
use crate::app::webserial::{SerialError, SerialMessage, SerialStatus};
use crate::app::{self, logging};

use w25q::series25::FlashInfo;

pub fn handler(
    command: SerialMessage,
    flash: &mut FlashT,
    flash_info: &mut FlashInfo,
    tables: &mut Tables,
) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: command.command,
        status: 0,
        payload: [0u8; 123],
        crc: 0,
    };

    // reversa:
    // let a = 4;
    // let b = 5;
    // let c = 0;
    // c = (a << 4);
    // c = c | b;

    logging::host::debug!(
        "Table A {} ; B {} ; C {}",
        command.status & 0b00001111,
        (command.status & 0b11110000) >> 4,
        command.command & 0b11110000
    );
    let selected_table = command.status & 0b00001111;

    match command.command & 0b00001111 {
        // table get metadata
        0x01 => {
            response_buf.payload[0] = 254;
            response_buf.payload[1] = selected_table;
            for _i in 0..20 {
                app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
            }
            logging::host::debug!("Table get metadata");
        }
        // get X table
        0x02 => {
            response_buf.payload[0] = 253;
            logging::host::debug!("Table get");
            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
        }
        // put X table
        0x03 => {
            response_buf.payload[0] = 253;
            logging::host::debug!("Table put");
            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
        }
        // write X table
        0x04 => {
            response_buf.payload[0] = 253;
            logging::host::debug!("Table write");

            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
        }
        _ => {
            app::send_message::spawn(
                SerialStatus::Error,
                SerialError::UnknownCmd as u8,
                response_buf,
            )
            .unwrap();
        }
    };
}
