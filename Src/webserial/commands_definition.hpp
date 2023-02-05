#ifndef WEBSERIAL_COMMANDS_DEFINITION_HPP
#define WEBSERIAL_COMMANDS_DEFINITION_HPP

#define CORE_PING 1
#define CORE_PONG 2
#define CORE_HELLO 10
#define CORE_STATUS 5           // RPM/Temp/Voltaje Bateria / avance / carga / MAP
#define CORE_STATUS_METADA 4    // devuelve max rpm / max avance

// 20 => get table metadata, 21 => get X table, 22 => response get X table,
// 23 => reset X table, 24 => write X table , 25 => response write x table
// 26 => begin data chunck , 27 => end data chunck (podrian ser un solo
// comando?)

// input:
#define TABLES_GET_METADATA 20
// retorna => [2b|int16 => x, 2b|int16 => y]

#define TABLES_GET 21
#define TABLES_RESET 23
#define TABLES_WRITE 24

// response:
#define TABLES_PUT 22    // esto tambien se usa para ir agregando data al array', primer byte el n°
#define TABLES_WRITE_OK 25
#define TABLES_DATA_CHUNK 26
#define TABLES_DATA_END_CHUNK 27
#define TABLES_INVALID_TABLE 28
#define TABLES_INVALID_SUBTABLE 29
#define TABLES_CRC_ERROR 30
#define TABLES_CRC_VALIDATE 31
#define TABLES_WRITE_FAIL 32

// esto llega en el payload (primeros 2byte, 16b valor), luego x2 bytes en Y
#define TABLES_IGNITION_TPS 10
#define TABLES_IGNITION_TEMP 11
#define TABLES_INJECTION_VE_MAIN 20
// debug commands

#define EFI_DEBUG_LOG 80
#define EFI_DEBUG_INFO 81
#define EFI_DEBUG_EVENT 82
#define EFI_DEBUG_ERROR 83
// enviado al final de cada mensaje para poder exceder el limite de 120 caracteres
#define EFI_DEBUG_END 89

// error commands:
#define EFI_INVALID_CODE 91
#define EFI_INVALID_CHECKSUM 92
#define EFI_INVALID_PROTOCOL 93

// User Fontend
#define FRONTEND_IN_COMMAND 66
#define FRONTEND_OUT_COMMAND 67
#define FRONTEND_DEBUG_MESSAGE 68
#define FRONTEND_ERROR_MESSAGE 69

// DEBUG
#define EFI_DEBUG_LOG 80
#define EFI_DEBUG_INFO 81
#define EFI_DEBUG_EVENT 82
#define EFI_DEBUG_ERROR 83
// enviado al final de cada mensaje para poder exceder el limite de 120 caracteres
#define EFI_DEBUG_END 89

// EFI_CONFIG
#define EFI_CONFIG_GET 100
#define EFI_CONFIG_WRITE 101
#define EFI_CONFIG_CHUNK 102
#define EFI_CONFIG_END 103
#define EFI_CONFIG_RESET 104
#define EFI_CONFIG_OK 105

// OTA
#define FW_BOOTLOADER 990
#define FW_REBOOT_UNSAFE 999
#endif