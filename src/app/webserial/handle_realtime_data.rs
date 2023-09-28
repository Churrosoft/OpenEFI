use rtic::Mutex;

use crate::app::{
    self,
    webserial::{SerialCode, SerialMessage, SerialStatus},
};

use crate::app::logging::host;

fn fmt_data(number: f32) -> i32 {
    (number * 100.0) as i32
}

pub async fn realtime_data_cdc_callback(mut ctx: app::realtime_data_cdc_callback::Context<'_>, serial_cmd: SerialMessage) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: serial_cmd.command,
        status: 0,
        code: 0,
        payload: [0u8; 122],
        crc: 0,
    };

    let mut number_arr: [u8; 4];
    match serial_cmd.command & 0b00001111 {
        // read dashboard Data:
        0x01 => {
            host::trace!("read dashboard cfg");
        }
        0x02 => {
            // RPM:
            number_arr = i32::to_le_bytes(ctx.shared.efi_status.lock(|es| es.rpm));
            response_buf.payload[..2].copy_from_slice(&number_arr[2..]);
            // TPS:
            number_arr = i32::to_le_bytes(fmt_data(ctx.shared.sensors.lock(|sns| sns.tps)));
            response_buf.payload[2..6].copy_from_slice(&number_arr);

            // MAP:
            number_arr = i32::to_le_bytes(fmt_data(ctx.shared.sensors.lock(|sns| sns.map)));
            response_buf.payload[6..10].copy_from_slice(&number_arr);

            // TEMP:
            number_arr = i32::to_le_bytes(fmt_data(ctx.shared.sensors.lock(|sns| sns.cooltan_temp)));
            response_buf.payload[10..14].copy_from_slice(&number_arr);

            // IAT:
            number_arr = i32::to_le_bytes(fmt_data(ctx.shared.sensors.lock(|sns| sns.air_temp)));
            response_buf.payload[14..18].copy_from_slice(&number_arr);

            // VBAT:
            number_arr = i32::to_le_bytes(fmt_data(ctx.shared.sensors.lock(|sns| sns.batt)));
            response_buf.payload[18..22].copy_from_slice(&number_arr);

            // // LMB
            // number_arr = i32::to_le_bytes(fmt_data(0.998)); //FIXME: get correct lambda value
            // response_buf.payload[22..26].copy_from_slice(&number_arr);
            //
            //
            // // 20b to ignition
            // // advance
            // number_arr = i32::to_le_bytes(fmt_data(23.6));
            // response_buf.payload[60..84].copy_from_slice(&number_arr);

            // 20b to injection:
            number_arr = i32::to_le_bytes(fmt_data(ctx.shared.efi_status.lock(|es| es.injection.injection_bank_1_time)));

            response_buf.payload[80..84].copy_from_slice(&number_arr);

            // base air
            number_arr = i32::to_le_bytes(fmt_data(ctx.shared.efi_status.lock(|es| es.injection.base_air)));
            response_buf.payload[84..88].copy_from_slice(&number_arr);

            // base fuel
            number_arr = i32::to_le_bytes(fmt_data(ctx.shared.efi_status.lock(|es| es.injection.base_fuel)));
            response_buf.payload[88..92].copy_from_slice(&number_arr);

            ctx.shared.crc.lock(|crc|
                {
                    crc.init();
                    let crc = crc.update_bytes(&response_buf.payload[..118]);
                    response_buf.payload[118..122].copy_from_slice(&crc.to_le_bytes());
                });
        }
        _ => {
            host::trace!("realtime data webserial error");
            app::send_message::spawn(
                SerialStatus::Error,
                SerialCode::UnknownCmd as u8,
                response_buf,
            ).unwrap();
            return;
        }
    };
    app::send_message::spawn(SerialStatus::Error, SerialCode::ParseError as u8, response_buf).unwrap();
}
