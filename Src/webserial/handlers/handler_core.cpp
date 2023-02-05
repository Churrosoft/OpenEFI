#include "../commands.hpp"
#include "defines.h"
#include "engine_status.hpp"
#include "handlers.hpp"
#include "sensors/sensors.hpp"


void handlers::handle_core(web_serial::serial_command command) {
  uint8_t payload[123];
  
  switch (command.command) {
    case WS_COMMAND::CORE_PING: {
      payload[0] = OPENEFI_BOARD_TYPE;
      
      payload[1] = (OPENEFI_VER_MAJOR >> 8) & 0xFF;
      payload[2] = OPENEFI_VER_MAJOR & 0xFF;

      payload[3] = (OPENEFI_VER_MINOR >> 8) & 0xFF;
      payload[4] = OPENEFI_VER_MINOR & 0xFF;

      payload[5] = (OPENEFI_VER_REV >> 8) & 0xFF;
      payload[6] = OPENEFI_VER_REV & 0xFF;
        
      web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_OK, payload);
      web_serial::paired = true;
      break;
    }

    case WS_COMMAND::CORE_STATUS_METADATA: {
      payload[0] = (MAX_RPM >> 8) & 0xFF;
      payload[1] = MAX_RPM & 0xFF;

      payload[2] = (TEMP_MAX_VALUE >> 8) & 0xFF;
      payload[3] = TEMP_MAX_VALUE & 0xFF;

      web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_OK, payload);
      break;
    }

    case WS_COMMAND::CORE_STATUS: {
      // int mockrpm = 750 + (rand() % 5750);
      uint32_t mockrpm = _RPM;
      uint32_t mocktemp = 1 + (rand() % 13000);
      uint32_t mockload = 1 + (rand() % 100);
      uint32_t mockbattery = 1 + (rand() % 1500);

      payload[0] = (mockrpm >> 8) & 0xFF;
      payload[1] = mockrpm & 0xFF;

      // legacy sensor status:
      payload[2] = (mocktemp >> 8) & 0xFF;
      payload[3] = mocktemp & 0xFF;

      payload[4] = (mockload >> 8) & 0xFF;
      payload[5] = mockload & 0xFF;

      payload[6] = (mockbattery >> 8) & 0xFF;
      payload[7] = mockbattery & 0xFF;

      payload[8] = (_AE >> 8) & 0xFF;
      payload[9] = _AE & 0xFF;

      // efi status
      payload[10] = 1;

      // estado / valores sensores;
      // -1 == sensor desconectado / no disponible

      // TPS simple/doble
      payload[11] = (uint8_t)mockload;
      payload[12] = (uint8_t)(mockload >> 8) & 0xFF;
      payload[13] = (uint8_t)(mockload >> 16) & 0xFF;
      payload[14] = (uint8_t)(mockload >> 24) & 0xFF;

      // MAP
      payload[15] = (uint8_t)sensors::values._MAP;
      payload[16] = (uint8_t)(sensors::values._MAP >> 8) & 0xFF;
      payload[17] = (uint8_t)(sensors::values._MAP >> 16) & 0xFF;
      payload[18] = (uint8_t)(sensors::values._MAP >> 24) & 0xFF;

      // TEMP
      payload[19] = (uint8_t)mocktemp;
      payload[20] = (uint8_t)(mocktemp >> 8) & 0xFF;
      payload[21] = (uint8_t)(mocktemp >> 16) & 0xFF;
      payload[22] = (uint8_t)(mocktemp >> 24) & 0xFF;

      // IAT
      payload[23] = (uint8_t)sensors::values.IAT;
      payload[24] = (uint8_t)(sensors::values.IAT >> 8) & 0xFF;
      payload[25] = (uint8_t)(sensors::values.IAT >> 16) & 0xFF;
      payload[26] = (uint8_t)(sensors::values.IAT >> 24) & 0xFF;

      // BATT
      payload[27] = (uint8_t)mockbattery;
      payload[28] = (uint8_t)(mockbattery >> 8) & 0xFF;
      payload[29] = (uint8_t)(mockbattery >> 16) & 0xFF;
      payload[30] = (uint8_t)(mockbattery >> 24) & 0xFF;

      // LMB
      payload[31] = (uint8_t)sensors::values.LMB;
      payload[32] = (uint8_t)(sensors::values.LMB >> 8) & 0xFF;
      payload[33] = (uint8_t)(sensors::values.LMB >> 16) & 0xFF;
      payload[34] = (uint8_t)(sensors::values.LMB >> 24) & 0xFF;

      // Ignition Status (20b)
      payload[60] = (uint8_t)_AE;
      payload[61] = (uint8_t)(_AE >> 8) & 0xFF;
      payload[62] = (uint8_t)(_AE >> 16) & 0xFF;
      payload[63] = (uint8_t)(_AE >> 24) & 0xFF;
      payload[80] = 1;

      // Injection Status (20b)
      payload[80] = (uint8_t)_INY_T1;
      payload[81] = (uint8_t)(_INY_T1 >> 8) & 0xFF;
      payload[82] = (uint8_t)(_INY_T1 >> 16) & 0xFF;
      payload[83] = (uint8_t)(_INY_T1 >> 24) & 0xFF;

      payload[84] = (uint8_t)_INY_T1;
      payload[85] = (uint8_t)(_INY_T1 >> 8) & 0xFF;
      payload[86] = (uint8_t)(_INY_T1 >> 16) & 0xFF;
      payload[87] = (uint8_t)(_INY_T1 >> 24) & 0xFF;

      payload[88] = (uint8_t)(int32_t)efi_status.injection.targetAFR;
      payload[89] = (uint8_t)((int32_t)efi_status.injection.targetAFR >> 8) & 0xFF;
      payload[90] = (uint8_t)((int32_t)efi_status.injection.targetAFR >> 16) & 0xFF;
      payload[91] = (uint8_t)((int32_t)efi_status.injection.targetAFR >> 24) & 0xFF;

      payload[100] = 1;

      web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_OK, payload);
      break;
    }
    default:
      web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::GENERIC_UNKNOWN_CMD, payload);
  }
}