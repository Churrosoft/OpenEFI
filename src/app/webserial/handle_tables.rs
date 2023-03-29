use crate::app;
use crate::app::webserial::{SerialError, SerialMessage, SerialStatus};

pub fn handler(command: SerialMessage) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: command.command,
        status: 0,
        payload: [0u8; 123],
        crc: 0,
    };
    let selected_table = command.command & 0b00001111;

    // reversa:
    // let a = 4;
    // let b = 5;
    // let c = 0;
    // c = (a << 4);
    // c = c | b;

    match (command.command & 0b11110000) >> 4 {
        // table get metadata
        0x01 => {
            response_buf.payload[0] = 254;
            response_buf.payload[1] = selected_table;
            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
        }
        // get X table
        0x02 => {
            response_buf.payload[0] = 253;

            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
        }
        // put X table
        0x03 => {
            response_buf.payload[0] = 253;

            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
        }
        // write X table
        0x04 => {
            response_buf.payload[0] = 253;

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
