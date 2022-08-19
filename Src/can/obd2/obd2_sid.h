/** @file */
#include "defines.h"
/**
 * @page OBD OBD SID definitions
 * en algun momento lo voy a hacer andar, pero tendria que importar todo este
 * archivo como una pagina
 * @verbinclude obd2_pid.h
 */

// OBD SID definitions from ISO15031-5 page 32

#ifndef OB2_SID_H
#define OB2_SID_H

/**
 * @brief SID $01 "Request current powertrain dignostic data request SID"
 */
#define REQUEST_AVAILABLE_SID 0x01

#define RESPONSE_AVAILABLE_SID 0x41 // SIDPR

#define SUPPORTED_SID01 0b00000000;
#define SUPPORTED_SID09 0b00000000;
#define SUPPORTED_SID11 0b00000000;
#define SUPPORTED_SID19 0b00000000;

#define SUPPORTED_SID21 0b00000000;
#define SUPPORTED_SID29 0b00000000;
#define SUPPORTED_SID31 0b00000000;
#define SUPPORTED_SID39 0b00000000;

/**
 * @brief SID $01 "Request powertrain freeze frame data"
 */
#define REQUEST_FREEZE_FRAME_SID 0x02

#define RESPONSE_FREEZE_FRAME_SID 0x42

#define SUPPORTED_PID01 0b00000000;
#define SUPPORTED_PID09 0b00000000;
#define SUPPORTED_PID11 0b00000000;
#define SUPPORTED_PID19 0b00000000;


#endif