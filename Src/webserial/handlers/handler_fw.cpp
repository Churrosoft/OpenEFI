#include "../commands.hpp"
#include "defines.h"
#include "engine_status.hpp"
#include "handlers.hpp"
#include "memory/include/config.hpp"
#include "sensors/sensors.hpp"


void handlers::handle_fw(web_serial::serial_command command) {
  uint8_t payload[123];
  
  switch (command.command) {
    case WS_COMMAND::FW_BOOTLOADER: {
      if(RPM::status != RPM_STATUS::STOPPED) {
        web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::FW_REBOOT_UNSAFE, payload);
        break;
      }
        
      // Write magic value to RAM and reset MCU
      *((volatile unsigned int*)0x20000000) = 0xb0d42b89;
      NVIC_SystemReset();
    }
    default:
      web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::GENERIC_UNKNOWN_CMD, payload);

  }
}