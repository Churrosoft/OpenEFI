#include "./obd2.hpp"

using namespace OBD2;

// read:
// https://stackoverflow.com/questions/29213988/flow-control-message-while-receiving-can-message-with-elm327/38891041#38891041

void OBD2::write_dtc_to_mem() { uint16_t example = __builtin_bswap16(34444); }

void OBD2::queue_can_message() {}

void OBD2::parse_can_message() {
  if (OBD2Mailbox.size()) {
    auto message = OBD2Mailbox.front();
    std::deque<uint8_t> txData;
    can_message tx;
    
    auto SID = message.frame_data.at(0).at(1);
    auto PID = message.frame_data.at(0).at(2);

    switch (SID) {

    case REQUEST_AVAILABLE_SID: {

      // primer parte (PID 00 | 20 ) son los SID disponibles
      // igual al ser por can (en teoria) solo tengo que reportar todos mis PID
      // disponibles de un saque

      tx.stdId = OBD2_CAN_ADDR_RESPONSE;
      tx.extId = 0;
      tx.is_extended_frame = false;
      // segun la norma el primer byte de data multi-frame es el comando que
      // responde
      txData[0] = RESPONSE_AVAILABLE_SID;

      txData[1] = 0;
      txData[2] = SUPPORTED_SID01;
      txData[3] = SUPPORTED_SID09;
      txData[4] = SUPPORTED_SID11;
      txData[5] = SUPPORTED_SID19;

      tx.frame_data.push_back(txData);

      txData[1] = 0x20;
      txData[2] = SUPPORTED_SID21;
      txData[3] = SUPPORTED_SID29;
      txData[4] = SUPPORTED_SID31;
      txData[5] = SUPPORTED_SID39;

      tx.frame_data.push_back(txData);

      // segunda parte, estado general (dtc/pids disponibles)

      txData[1] = 0x01;
      txData[2] = 0xff; // MIL status && DTC count
      txData[3] = 0xff; // Fuel system status
      txData[4] = 0xff; // CAT data 1
      txData[5] = 0xff; // CAT data 2

      tx.frame_data.push_back(txData);

      // send command:
      TxMailbox.push_front(tx);
      break;
    }

    case REQUEST_FREEZE_FRAME_SID: {
      // TODO: respuesta valida, pero por ahora la ecu no controla casos de
      // emergencia (por ej, falla en ckp impidiendo arranque)
      /*   txData[0] = RESPONSE_FREEZE_FRAME_SID;
        send_can_message(txData, 6);
        memset(txData, 0x0, 7); */
      break;
    }
    default:
      break;
    }
  }
}