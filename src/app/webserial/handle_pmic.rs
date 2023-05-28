use crate::app::{
    self, logging::host,
    webserial::{SerialError, SerialMessage, SerialStatus},
};
use crate::app::engine::pmic::PmicT;

pub fn handler(
    command: SerialMessage,
    pmic_instance: &mut PmicT,
) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: command.command,
        status: 0,
        payload: [0u8; 123],
        crc: 0,
    };

    let mut json_payload = [0u8; 350];
    let result;

    match command.command & 0b00001111 {
        // get all status
        0x01 => {
            host::trace!("PMIC: get fast status");
            let data = pmic_instance.get_fast_status();
            result = serde_json_core::to_slice(&data, &mut json_payload);
        }
        // get injection status
        0x02 => {
            host::trace!("PMIC: get injection status");
            let data = pmic_instance.get_injector_status();
            result = serde_json_core::to_slice(&data, &mut json_payload);
        }
        // get ignition status
        0x03 => {
            host::trace!("PMIC: get ignition status");
            let data = pmic_instance.get_ignition_status();
            result = serde_json_core::to_slice(&data, &mut json_payload);
        }
        _ => {
            app::send_message::spawn(
                SerialStatus::Error,
                SerialError::UnknownCmd as u8,
                response_buf,
            )
                .unwrap();
            return;
        }
    }

    if result.is_ok_and(|s| s > 0) {
        let command_count = result.unwrap().div_ceil(123);

        for i in 0..command_count {
            let from = i * 123;
            let to = from + 123;
            response_buf.payload.copy_from_slice(&json_payload[from..to]);
            app::send_message::spawn(SerialStatus::DataChunk, 0, response_buf).unwrap();
            response_buf.payload.fill(0x0);
        }
        app::send_message::spawn(SerialStatus::DataChunkEnd, 0, response_buf).unwrap();
        return;
    }
    app::send_message::spawn(SerialStatus::Error, SerialError::ParseError as u8, response_buf).unwrap();
}
