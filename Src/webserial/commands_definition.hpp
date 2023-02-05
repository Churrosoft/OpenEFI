#ifndef WEBSERIAL_COMMANDS_DEFINITION_HPP
#define WEBSERIAL_COMMANDS_DEFINITION_HPP

namespace webserial {
  enum _webserial_command_types {
    CORE        = 0x00,
    TABLES      = 0x10,
    EFI_CONFIG  = 0x20,
    
    FW          = 0xf0,
  };
  
  enum _webserial_command {
    // Core
    CORE_DEBUG            = 0x00,  // For debug messages. Not an actual command that we handle.
    CORE_PING             = 0x01,
    CORE_STATUS_METADATA  = 0x04,
    CORE_STATUS           = 0x05,
    
    // Tables
    TABLES_GET_METADATA   = 0x10,
    TABLES_GET            = 0x11,
    TABLES_PUT            = 0x12,
//    TABLES_RESET          = 0x13, (unused)
    TABLES_WRITE          = 0x14,
    
    // EFI_CONFIG
    EFI_CONFIG_GET        = 0x20,
    EFI_CONFIG_WRITE      = 0x21,
    EFI_CONFIG_RESET      = 0x22,
    
    // FW
    FW_BOOTLOADER         = 0xf1
  };
  
  enum _webserial_command_status {
    CMD_ERR   = 0b00000000,
    CMD_OK    = 0b10000000,
  };
  
  enum _webserial_error_code {
    GENERIC_UNKNOWN_CMD   = 0x7f,
    // Core
    //.. (none)
    
    // Tables
    TABLES_INVALID_TABLE  = 0x01,
    TABLES_CRC_ERROR      = 0x02,
    TABLES_WRITE_FAIL     = 0x03,
    
    // FW
    FW_REBOOT_UNSAFE      = 0x01,
  };
}
typedef webserial::_webserial_command WS_COMMAND;
typedef webserial::_webserial_command_types WS_COMMAND_TYPE;
typedef webserial::_webserial_command_status WS_COMMAND_STATUS;
typedef webserial::_webserial_error_code WS_COMMAND_ERROR;


// esto llega en el payload (primeros 2byte, 16b valor), luego x2 bytes en Y
#define TABLES_IGNITION_TPS 10  // Not a command
#define TABLES_IGNITION_TEMP 11
#define TABLES_INJECTION_VE_MAIN 20

#endif