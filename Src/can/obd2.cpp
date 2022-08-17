#include "./obd2.hpp"

using namespace OBD2;

// read:
// https://stackoverflow.com/questions/29213988/flow-control-message-while-receiving-can-message-with-elm327/38891041#38891041

void OBD2::write_dtc_to_mem() { uint16_t example = __builtin_bswap16(34444); }

void OBD2::queue_can_message() {}

void OBD2::parse_can_message() {
  if (pending_commands.size()) {
    can_command command = pending_commands.front();
    uint8_t txData[7];

    switch (command.command) {

    case REQUEST_AVAILABLE_SID: {
      txData[0] = RESPONSE_AVAILABLE_SID;

      // primer parte (PID 00 | 20 ) son los SID disponibles
      uint8_t PID = command.payload[1];

      switch (PID) {
      case 0x0:
        txData[1] = 0;
        txData[2] = SUPPORTED_SID01;
        txData[3] = SUPPORTED_SID09;
        txData[4] = SUPPORTED_SID11;
        txData[5] = SUPPORTED_SID19;      
        break;

      case 0x20:
        txData[1] = 0x20;
        txData[2] = SUPPORTED_SID21;
        txData[3] = SUPPORTED_SID29;
        txData[4] = SUPPORTED_SID31;
        txData[5] = SUPPORTED_SID39;
        break;

      // segunda parte, estado general (dtc/pids disponibles)
      case 0x01:
        txData[1] = 0x01;
        txData[2] = 0xff; // MIL status && DTC count
        txData[3] = 0xff; // Fuel system status
        txData[4] = 0xff; // CAT data 1
        txData[5] = 0xff; // CAT data 2
        break;
      default:
        // TODO: creeooo que la norma especificaba algo para estos casos, (no
        // msg de error, sino "rango no soportado")
        break;
      }

      // Para no bloquear el USB en comandos con frame-control se puede mandar
      // por partes
      send_can_message(txData, 6);
      memset(txData, 0x0, 7);
      break;
    }

    case REQUEST_FREEZE_FRAME_SID: {
      // TODO: respuesta valida, pero por ahora la ecu no controla casos de
      // emergencia (por ej, falla en ckp impidiendo arranque)
      txData[0] = RESPONSE_FREEZE_FRAME_SID;
      send_can_message(txData, 6);
      memset(txData, 0x0, 7);
    }
    default:
      break;
    }
  }
}