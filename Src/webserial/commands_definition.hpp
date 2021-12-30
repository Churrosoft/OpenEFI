#include "defines.h"

#ifndef WEBSERIAL_COMMANDS_DEFINITION_HPP
#define WEBSERIAL_COMMANDS_DEFINITION_HPP

/* // 20 => get table metadata, 21 => get X table, 22 => response get X table,
// 23 => reset X table, 24 => write X table , 25 => response write x table
// 26 => begin data chunck , 27 => end data chunck (podrian ser un solo comando?)
type tableCommands = 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27;
 */
// input:
#define TABLES_GET_METADATA 20
// retorna => [2b|int16 => x, 2b|int16 => y]

#define TABLES_GET 21
#define TABLES_RESET 23
#define TABLES_WRITE 24

// response:
#define TABLES_PUT 22
#define TABLES_WRITE_OK 25
#define TABLES_DATA_CHUNK 26
#define TABLES_INVALID_TABLE 28
#define TABLES_INVALID_SUBTABLE 29

// esto llega en el payload (primeros 2byte, 16b valor), luego x2 bytes en Y
#define TABLES_IGNITION_TPS 10
#define TABLES_IGNITION_TEMP 11

// error commands:
#define EFI_INVALID_CODE 91
#define EFI_INVALID_CHECKSUM 92
#define EFI_INVALID_PROTOCOL 93

#endif