use postcard::{from_bytes, to_vec};
use rtic::Mutex;
use rtic::mutex_prelude::*;
use serde_json_core::heapless::Vec;

use crate::app::{
    self,
    webserial::{SerialCode, SerialMessage, SerialStatus},
};
use crate::app::engine::efi_cfg::EngineConfig;
use crate::app::logging::host;

pub async fn engine_cdc_callback(mut ctx: app::engine_cdc_callback::Context<'_>, serial_cmd: SerialMessage) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: serial_cmd.command,
        status: 0,
        code: 0,
        payload: [0u8; 122],
        crc: 0,
    };

    let mut json_payload = [0u8; 3500];
    let mut result;

    let mut flash = ctx.shared.flash;
    let mut flash_info = ctx.shared.flash_info;
    let mut efi_cfg = ctx.shared.efi_cfg;
    let mut crc = ctx.shared.crc;
    let efi_cfg_snapshot = efi_cfg.lock(|c| c.clone());

    if !efi_cfg_snapshot.ready {
        host::trace!("engine cfg webserial error");
        app::send_message::spawn(
            SerialStatus::Error,
            SerialCode::ParseError as u8,
            response_buf,
        ).unwrap();
        return;
    }


    match serial_cmd.command & 0b00001111 {
        // read engine cfg:
        0x01 => {
            host::trace!("read engine cfg");
            result = serde_json_core::to_slice(&efi_cfg_snapshot, &mut json_payload);
        }
        0x02 => {
            //TODO: se puede controlar el caso de que falle la grabacion
            (efi_cfg, flash, flash_info, crc).lock(|efi_cfg, flash, flash_info, crc| efi_cfg.save(flash, flash_info, crc));

            app::send_message::spawn(
                SerialStatus::Ok,
                SerialCode::None as u8,
                response_buf,
            ).unwrap();
            return;
        }
        _ => {
            host::trace!("engine cfg webserial error");
            app::send_message::spawn(
                SerialStatus::Error,
                SerialCode::UnknownCmd as u8,
                response_buf,
            ).unwrap();
            return;
        }
    };

    if result.is_ok_and(|s| s > 0) {
        let command_count = result.unwrap().div_ceil(122);

        for i in 0..command_count {
            let from = i * 122;
            let to = from + 122;
            response_buf.payload.copy_from_slice(&json_payload[from..to]);

            response_buf.status = SerialStatus::DataChunk as u8;
            response_buf.code = 0;

            app::send_message::spawn(SerialStatus::DataChunk, 0, response_buf).unwrap();

            response_buf.payload.fill(0x0);
        }


        app::send_message::spawn(SerialStatus::DataChunkEnd, 0, response_buf).unwrap();

        let output: Vec<u8, 800> = to_vec(&efi_cfg_snapshot).unwrap();
        let mut cfg_new: EngineConfig = from_bytes(&output).unwrap();
        result = serde_json_core::to_slice(&cfg_new, &mut json_payload);
        host::trace!("struct => bytes => struct => json result {:?}",result);

        return;
    }


    app::send_message::spawn(SerialStatus::Error, SerialCode::ParseError as u8, response_buf).unwrap();
}
