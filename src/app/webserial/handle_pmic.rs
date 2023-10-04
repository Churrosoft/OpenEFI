use rtic::Mutex;
use crate::app::{
    self, // logging::host,
    webserial::{SerialCode, SerialMessage, SerialStatus},
};
pub async fn pmic_cdc_callback(mut ctx: app::pmic_cdc_callback::Context<'_>, serial_cmd: SerialMessage) {
    let mut response_buf = SerialMessage {
        protocol: 1,
        command: serial_cmd.command,
        status: 0,
        code: 0,
        payload: [0u8; 122],
        crc: 0,
    };

    let mut json_payload = [0u8; 350];
    let result;

    match serial_cmd.command & 0b00001111 {
        // get all status
        0x01 => {
            // host::trace!("PMIC: get fast status");
            let data = ctx.shared.pmic.lock(|pmic|pmic.get_fast_status());
            result = serde_json_core::to_slice(&data, &mut json_payload);
        }
        // get injection status
        0x02 => {
            // host::trace!("PMIC: get injection status");
            let data =  ctx.shared.pmic.lock(|pmic|pmic.get_injector_status());
            result = serde_json_core::to_slice(&data, &mut json_payload);
        }
        // get ignition status
        0x03 => {
            // host::trace!("PMIC: get ignition status");
            let data =ctx.shared.pmic.lock(|pmic|pmic.get_ignition_status());
            result = serde_json_core::to_slice(&data, &mut json_payload);
        }
        _ => {
            response_buf.status = SerialStatus::Error as u8;
            response_buf.code = SerialCode::UnknownCmd as u8;

            let _ = ctx.local.pmic_sender.send(response_buf).await;
            return;
        }
    }

    if result.is_ok_and(|s| s > 0) {
        let command_count = result.unwrap().div_ceil(122);

        for i in 0..command_count {
            let from = i * 122;
            let to = from + 122;
            response_buf.payload.copy_from_slice(&json_payload[from..to]);

            response_buf.status = SerialStatus::DataChunk as u8;
            response_buf.code = 0;
            let _ = ctx.local.pmic_sender.send(response_buf).await;

            response_buf.payload.fill(0x0);
        }
        response_buf.status = SerialStatus::DataChunkEnd as u8;
        response_buf.code = 0;
        let _ = ctx.local.pmic_sender.send(response_buf).await;
        return;
    }

    response_buf.status = SerialStatus::Error as u8;
    response_buf.code = SerialCode::ParseError as u8;

    let _ = ctx.local.pmic_sender.send(response_buf).await;
}
