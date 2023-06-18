use stm32f4xx_hal::crc32::Crc32;
use w25q::series25::FlashInfo;

use crate::app::{
    self,
    memory::tables::{FlashT, TableData, Tables},
    webserial::{SerialCode, SerialMessage, SerialStatus},
};
use crate::app::engine::efi_cfg::EngineConfig;

use crate::app::logging::host;

pub fn handler(
    flash: &mut FlashT,
    flash_info: &mut FlashInfo,
    crc: &mut Crc32,
    efi_cfg: &mut EngineConfig,
    command: SerialMessage) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: command.command,
        status: 0,
        code: 0,
        payload: [0u8; 122],
        crc: 0,
    };

    let mut json_payload = [0u8; 3500];
    let result;

    match command.command & 0b00001111 {
        // read engine cfg:
        0x01 => {
            host::trace!("read engine cfg");
            result = serde_json_core::to_slice(&efi_cfg, &mut json_payload);
        }
        _ => {
            host::trace!("engine cfg webserial error");
            app::send_message::spawn(
                SerialStatus::Error,
                SerialCode::UnknownCmd as u8,
                response_buf,
            )
                .unwrap();
            return;
        }
    };

    if result.is_ok_and(|s| s > 0) {
        let command_count = result.unwrap().div_ceil(122);

        for i in 0..command_count {
            let from = i * 122;
            let to = from + 122;
            response_buf.payload.copy_from_slice(&json_payload[from..to]);
            app::send_message::spawn(SerialStatus::DataChunk, 0, response_buf).unwrap();
            response_buf.payload.fill(0x0);
        }
        app::send_message::spawn(SerialStatus::DataChunkEnd, 0, response_buf).unwrap();
        return;
    }
    app::send_message::spawn(SerialStatus::Error, SerialCode::ParseError as u8, response_buf).unwrap();
}
