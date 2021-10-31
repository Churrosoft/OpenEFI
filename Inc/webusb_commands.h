#ifndef COMMANDS_H
#define COMMANDS_H

#define PROTOCOL_VERSION_1 1
// comandos del protocolo
#define COMMAND_PING 1
// Comando de estado:
#define COMMAND_STATUS 2
#define COMMAND_DBG 8
// Enviado cuando ocurre un error. El error se especifica en el subcomando
#define COMMAND_ERR 9
#define ERROR_INVALID_PROTOCOL 1
#define ERROR_INVALID_COMMAND 2
#define ERROR_INVALID_CHECKSUM 3
#define COMMAND_HELLO 10
// Intenta reiniciar a dapboot.
#define COMMAND_BOOTL_SW 11

#define SUBCOMMAND_NONE 0
/*
  * subcomandos para comandos de estado y spam
*/
// XXX: Definir si vamos a usar subcomandos o un comando que envie todos los datos
// #define STATUS_TMP 1
// #define STATUS_RPM 2
// #define STATUS_00V 3
// #define STATUS_AVC 4
// #define STATUS_INY 5
// #define STATUS_LMB 6

#endif