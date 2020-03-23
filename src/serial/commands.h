#ifndef COMMANDS_H
#define COMMANDS_H

#define PROTOCOL_VERSION_1 1

#define COMMAND_PING 1
#define COMMAND_TMP 2
#define COMMAND_RPM 3
#define COMMAND_00V 4
#define COMMAND_AVC 5
#define COMMAND_INY 6
#define COMMAND_LMB 7
#define COMMAND_DBG 8
// Enviado cuando ocurre un error. El error se especifica en el subcomando
#define COMMAND_ERR 9
#define ERROR_INVALID_PROTOCOL 1
#define ERROR_INVALID_COMMAND 2
#define ERROR_INVALID_CHECKSUM 3
#define COMMAND_HELLO 10

#define SUBCOMMAND_NONE 0

#endif