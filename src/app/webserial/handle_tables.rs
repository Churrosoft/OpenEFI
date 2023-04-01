use crate::app::memory::tables::{FlashT, Tables};
use crate::app::webserial::{SerialError, SerialMessage, SerialStatus};
use crate::app::{self, logging};

use stm32f4xx_hal::crc32::Crc32;
use w25q::series25::FlashInfo;

pub fn handler(
    command: SerialMessage,
    flash: &mut FlashT,
    flash_info: &mut FlashInfo,
    tables: &mut Tables,
    crc: &mut Crc32,
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
            //TODO: faltaria un objeto global de poneleque solo lectura para traer toda la metadata de las tablas
            // ya si hago mutable el struct de tablas con la data adentro rompe por lifetime

            match selected_table {
                0x01 => {
                    // TPS RPM VE
                    response_buf.payload[0] = 17;
                    response_buf.payload[1] = 17;
                    logging::host::debug!("Table get metadata. TPS VE");
                }
                _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialError::UnknownTable as u8,
                        response_buf,
                    )
                    .unwrap();
                    return;
                }
            }
        }
        // get X table
        0x02 => {
            #[allow(unused_assignments)]
            let mut table = [[0i32; 17]; 17];

            match selected_table {
                0x01 => {
                    // TPS RPM VE
                    logging::host::debug!("Table get. TPS VE");
                    // TODO: read table if not read prev.
                    table = tables.tps_rpm_ve.unwrap();
                }
                _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialError::UnknownTable as u8,
                        response_buf,
                    )
                    .unwrap();
                    return;
                }
            }

            for row in table {
                let mut i = 0;

                for row_number in row {
                    let number_arr: [u8; 4] = i32::to_le_bytes(row_number);
                    response_buf.payload[i] = number_arr[0];
                    response_buf.payload[i + 1] = number_arr[1];
                    response_buf.payload[i + 2] = number_arr[2];
                    response_buf.payload[i + 3] = number_arr[3];
                    i += 4;
                }

                app::send_message::spawn(SerialStatus::DataChunk, 0, response_buf).unwrap();
            }
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
