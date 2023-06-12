use stm32f4xx_hal::crc32::Crc32;
use w25q::series25::FlashInfo;

use crate::app::{
    self, logging,
    memory::tables::{FlashT, TableData, Tables},
    webserial::{SerialCode, SerialMessage, SerialStatus},
};

pub fn handler(
    command: SerialMessage,
    flash: &mut FlashT,
    flash_info: &mut FlashInfo,
    table_data: &mut Tables,
    crc: &mut Crc32,
) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: 0x10,
        status: 0,
        payload: [0u8; 123],
        crc: 0,
    };

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
                    logging::host::debug!("Table get metadata - TPS VE");
                }
                _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
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
                    // TODO: read table if not read prev.
                    table = table_data.tps_rpm_ve.unwrap();
                }
                _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    )
                        .unwrap();
                    return;
                }
            }

            for row in table {
                let mut row_index = 0;
                let mut i = 1;
                response_buf.payload[0] = row_index;

                for row_number in row {
                    let number_arr: [u8; 4] = i32::to_le_bytes(row_number);
                    response_buf.payload[i] = number_arr[0];
                    response_buf.payload[i + 1] = number_arr[1];
                    response_buf.payload[i + 2] = number_arr[2];
                    response_buf.payload[i + 3] = number_arr[3];
                    i += 4;
                }
                row_index += 1;
                app::send_message::spawn(SerialStatus::DataChunk, 0, response_buf).unwrap();
            }
            app::send_message::spawn(SerialStatus::DataChunkEnd, 0, response_buf).unwrap();
        }
        // put X table
        0x03 => {
            let mut row_data = 1;
            let row_index = command.payload[0] as usize;
            let mut table_row = [0i32; 17];
            #[allow(unused_assignments)]
                let mut table = [[0i32; 17]; 17];

            // FIXME: get max index from correct table
            for matrix_x in 0..17 {
                let u8buff = [
                    command.payload[row_data],
                    command.payload[row_data + 1],
                    command.payload[row_data + 2],
                    command.payload[row_data + 3],
                ];

                let value = i32::from_le_bytes(u8buff);
                table_row[matrix_x] = value;
                row_data += 4;
            }

            match selected_table {
                0x01 => {
                    // TPS RPM VE
                    // TODO: read table if not read prev.
                    table = table_data.tps_rpm_ve.unwrap();
                    table[row_index] = table_row;
                    table_data.tps_rpm_ve = Some(table);
                    app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
                    return;
                }
                _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    )
                        .unwrap();
                    return;
                }
            }
        }
        // write X table
        // TODO: check crc32 prev write
        0x04 => {
            response_buf.payload[0] = 253;

            match selected_table {
                0x01 => {
                    // TPS RPM VE
                    let tps_rpm_ve = TableData {
                        data: table_data.tps_rpm_ve,
                        crc: 0,
                        address: 0x3,
                        max_x: 17,
                        max_y: 17,
                    };

                    if table_data.tps_rpm_ve.is_some() {
                        tps_rpm_ve.write_to_memory(flash, flash_info, crc);
                        app::send_message::spawn(SerialStatus::UploadOk, 0, response_buf).unwrap();
                        return;
                    }

                    logging::host::debug!("Table Write TableNotLoaded - TPS VE");

                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::TableNotLoaded as u8,
                        response_buf,
                    )
                        .unwrap();
                    return;
                }
                _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    )
                        .unwrap();
                    return;
                }
            }
        }

        // Clear selected table
        0x09 => {
            match selected_table {
                0x01 => {
                    // TPS RPM VE
                    let mut tps_rpm_ve = TableData {
                        data: table_data.tps_rpm_ve,
                        crc: 0,
                        address: 0x3,
                        max_x: 17,
                        max_y: 17,
                    };

                    tps_rpm_ve.clear(flash, flash_info, crc);
                    logging::host::debug!("Table Clear TPS/RPM/VE [injection]");

                    app::send_message::spawn(
                        SerialStatus::Ok,
                        SerialCode::None as u8,
                        response_buf,
                    )
                        .unwrap();
                    return;
                }
                _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    )
                        .unwrap();
                    return;
                }
            }
        }
        _ => {
            app::send_message::spawn(
                SerialStatus::Error,
                SerialCode::UnknownCmd as u8,
                response_buf,
            )
                .unwrap();
            return;
        }
    }
}
