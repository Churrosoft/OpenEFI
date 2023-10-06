use rtic::Mutex;
use rtic::mutex_prelude::{TupleExt03, TupleExt06};
use rtic_monotonics::systick::*;
use stm32f4xx_hal::crc32::Crc32;
use w25q::series25::FlashInfo;

use crate::app::{
    self, logging,
    memory::tables::{FlashT, TableData, Tables},
    webserial::{SerialCode, SerialMessage, SerialStatus},
};

pub(crate) async fn table_cdc_callback(mut ctx: app::table_cdc_callback::Context<'_>, command: SerialMessage) {
    let flash = ctx.shared.flash;
    let flash_info = ctx.shared.flash_info;
    let mut tables = ctx.shared.tables;
    let crc = ctx.shared.crc;
    let mut cdc_sender = ctx.local.table_sender;


    let mut response_buf = SerialMessage {
        protocol: 1,
        command: 0x10,
        code: 0,
        status: 0,
        payload: [0u8; 122],
        crc: 0,
    };

    let selected_table = command.code;


    match command.command & 0b00001111 {
        // table get metadata
        0x01 => {
            //TODO: faltaria un objeto global de poneleque solo lectura para traer toda la metadata de las tablas
            // ya si hago mutable el struct de tablas con la data adentro rompe por lifetime

            match selected_table {
                0x01 => {
                    // TPS RPM VE (injection)
                    response_buf.payload[0] = 17;
                    response_buf.payload[1] = 17;
                    logging::host::debug!("Table get metadata - TPS VE");
                }
                0x10 => {
                    // TPS / LOAD / DEG (ignition)
                    response_buf.payload[0] = 17;
                    response_buf.payload[1] = 17;
                }
                0x2..0x9 | _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    ).unwrap();
                    return;
                }
            }
        }
        // get X table
        0x02 => {
            let mut table = [[0i32; 17]; 17];
            // logging::host::debug!("Table get");
            match selected_table {
                0x01 => {
                    // TODO: read table if not read prev.
                    table = tables.lock(|table_data| table_data.tps_rpm_ve.unwrap_or(table));
                }
                0x10 => {
                    // TODO: read table if not read prev.
                    table = tables.lock(|table_data| table_data.load_tps_deg.unwrap_or(table));
                }
                0x2..0x9 | _ => {
                    logging::host::debug!("Table unkown get");
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    ).unwrap();
                    return;
                }
            }

            for row in table {
                let mut _row_index = 0;
                let mut i = 1;
                response_buf.payload[0] = _row_index;

                for row_number in row {
                    let number_arr: [u8; 4] = i32::to_le_bytes(row_number);
                    response_buf.payload[i] = number_arr[0];
                    response_buf.payload[i + 1] = number_arr[1];
                    response_buf.payload[i + 2] = number_arr[2];
                    response_buf.payload[i + 3] = number_arr[3];
                    i += 4;
                }
                _row_index += 1;
                response_buf.status = SerialStatus::DataChunk as u8;
                response_buf.code = 0;
                cdc_sender.send(response_buf.clone()).await.unwrap();
               Systick::delay(10.millis()).await;
            }

            response_buf.status = SerialStatus::DataChunkEnd as u8;
            response_buf.code = 0;
            cdc_sender.send(response_buf.clone()).await.unwrap();
        }

        // put X table
        // TODO: read table if not read prev.
        0x03 => {
            let mut row_data = 1;
            let row_index = command.payload[0] as usize;
            let mut table_row = [0i32; 17];
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
                    table = tables.lock(|table_data| table_data.tps_rpm_ve.unwrap_or(table));
                    table[row_index] = table_row;
                    tables.lock(|table_data| table_data.tps_rpm_ve = Some(table));
                    app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
                    return;
                }
                0x02 => {
                    // TPS RPM AFR
                    table = tables.lock(|table_data| table_data.tps_rpm_afr.unwrap_or(table));
                    table[row_index] = table_row;
                    tables.lock(|table_data| table_data.tps_rpm_afr = Some(table));
                    app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
                    return;
                }
                0x10 => {
                    // TPS RPM VE
                    table = tables.lock(|table_data| table_data.tps_rpm_ve.unwrap_or(table));
                    table[row_index] = table_row;
                    tables.lock(|table_data| table_data.tps_rpm_ve = Some(table));
                    app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
                    return;
                }
                0x3..0x9 | _ => {
                    logging::host::debug!("error on get");
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    ).unwrap();
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
                        data: tables.lock(|tables| tables.tps_rpm_ve),
                        crc: 0,
                        address: 0x3,
                        max_x: 17,
                        max_y: 17,
                    };

                    if tps_rpm_ve.data.is_some() {
                        (flash, flash_info, crc).lock(|flash, flash_info, crc| {
                            tps_rpm_ve.write_to_memory(flash, flash_info, crc);
                        });

                        app::send_message::spawn(SerialStatus::UploadOk, 0, response_buf).unwrap();
                        return;
                    }

                    logging::host::debug!("Table Write TableNotLoaded - TPS VE");

                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::TableNotLoaded as u8,
                        response_buf,
                    ).unwrap();
                    return;
                }
                0x02 => {
                    // TPS RPM AFR
                    let tps_rpm_afr = TableData {
                        data: tables.lock(|tables| tables.tps_rpm_afr),
                        crc: 0,
                        address: 0x3,
                        max_x: 17,
                        max_y: 17,
                    };

                    if tps_rpm_afr.data.is_some() {
                        (flash, flash_info, crc).lock(|flash, flash_info, crc| {
                            tps_rpm_afr.write_to_memory(flash, flash_info, crc);
                        });


                        // tps_rpm_ve.write_to_memory(flash, flash_info, crc);
                        app::send_message::spawn(SerialStatus::UploadOk, 0, response_buf).unwrap();
                        return;
                    }

                    logging::host::debug!("Table Write TableNotLoaded - TPS AFR");

                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::TableNotLoaded as u8,
                        response_buf,
                    ).unwrap();
                    return;
                }

                0x10 => {
                    // TPS RPM VE
                    let load_tps_deg = TableData {
                        data: tables.lock(|tables| tables.load_tps_deg),
                        crc: 0,
                        address: 0x3,
                        max_x: 17,
                        max_y: 17,
                    };

                    if load_tps_deg.data.is_some() {
                        (flash, flash_info, crc).lock(|flash, flash_info, crc| {
                            load_tps_deg.write_to_memory(flash, flash_info, crc);
                        });
                        app::send_message::spawn(SerialStatus::UploadOk, 0, response_buf).unwrap();
                        return;
                    }

                    logging::host::debug!("Table Write TableNotLoaded - LOAD TPS DEG");

                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::TableNotLoaded as u8,
                        response_buf,
                    ).unwrap();
                    return;
                }

                0x3..0x9 | _ => {
                    logging::host::debug!("error on write");
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    ).unwrap();
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
                        data: None,
                        crc: 0,
                        address: 0x3,
                        max_x: 17,
                        max_y: 17,
                    };

                    (flash, flash_info, crc).lock(|flash, flash_info, crc| {
                        tps_rpm_ve.clear(flash, flash_info, crc);
                    });

                    logging::host::debug!("Table Clear TPS/RPM/VE [injection]");

                    app::send_message::spawn(
                        SerialStatus::Ok,
                        SerialCode::None as u8,
                        response_buf,
                    ).unwrap();
                    return;
                }
                0x10 => {
                    // TPS RPM VE
                    let mut tps_rpm_ve = TableData {
                        data: None,
                        crc: 0,
                        address: 0x3,
                        max_x: 17,
                        max_y: 17,
                    };

                    (flash, flash_info, crc).lock(|flash, flash_info, crc| {
                        tps_rpm_ve.clear(flash, flash_info, crc);
                    });
                    logging::host::debug!("Table Clear LOAD/TPS/DEG [ignition]");

                    app::send_message::spawn(
                        SerialStatus::Ok,
                        SerialCode::None as u8,
                        response_buf,
                    ).unwrap();
                    return;
                }
                0x2..0x9 | _ => {
                    app::send_message::spawn(
                        SerialStatus::Error,
                        SerialCode::UnknownTable as u8,
                        response_buf,
                    ).unwrap();
                    return;
                }
            }
        }
        _ => {
            app::send_message::spawn(
                SerialStatus::Error,
                SerialCode::UnknownCmd as u8,
                response_buf,
            ).unwrap();
            return;
        }
    }
}