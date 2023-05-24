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

    match command.command & 0b00001111 {
        // get all status
        0x01 => {
            host::debug!("PMIC: get fast status");
            let data = pmic_instance.get_fast_status();
            host::debug!("{:?}",data);
            host::debug!("------------------");
            // de esto me sirven 300b clavados en el pero de los casos, serian 3 comandos de data
            let result = serde_json_core::to_slice(&data, &mut json_payload).unwrap();
            host::debug!("size: {:?}",result);

            response_buf.payload[0] = 0xff;
            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
        }
        // get injection status
        0x02 => {
            host::debug!("PMIC: get injection status");
            pmic_instance.get_injector_status();
            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
        }
        // get ignition status
        0x03 => {
            host::debug!("PMIC: get ignition status");
            pmic_instance.get_ignition_status();
            app::send_message::spawn(SerialStatus::Ok, 0, response_buf).unwrap();
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
}
