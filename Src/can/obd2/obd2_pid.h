/** @file */
#include "defines.h"
/**
 * @page OBD OBD PID definitions
 * en algun momento lo voy a hacer andar, pero tendria que importar todo este
 * archivo como una pagina
 * @verbinclude obd2_pid.h
 */

// OBD PID definitions from ISO15031-5 page 115
// only for services $01 | $02 | $03 | $04

#ifndef OB2_PID_H
#define OB2_PID_H

/**
 * @brief PID $03 "Fuel system (1 | 2) status"
 */
#define FUEL_SYSTEM_STATUS 0x03

/**
 * Motor off
 * @brief Motor off
 */
#define FS_MOTOR_OFF 0b00000000

/**
 * Open loop - has not yet satisfied conditions to
 * go closed loop
 * @brief Fuel System Open Loop
 */
#define FS_OPEN_LOOP 0b00000001 // 0b10000000

/**
 * Closed loop - using oxygen sensor(s) as
 * feedback for fuel control
 * @brief Fuel System Closed Loop
 */
#define FS_CLOSED_LOOP 0b00000010 // 0b01000000

/**
 * Open loop due to driving conditions (e.g.
 * power enrichment, deceleration enleanment)
 * @brief Fuel System Open Loop (fault)
 */
#define FS_OL_DRIVE 0b00000100 // 0b01000000

/**
 * Open loop - due to detected system fault
 * @brief Fuel System Open Loop (fault)
 */
#define FS_OL_FAULT 0b00001000 // 0b00010000

/**
 * Open loop - due to detected system fault
 * @brief Fuel System Closed Loop (fault)
 */
#define FS_CL_FAULT 0b00010000 // 0b00001000

/**
 * @brief PID $0C "Engine RPM"
 */
#define ENGINE_RPM 0x0C

/**
 * @see page 123 Table B.13
 * @return 16b with rpm on 1/4 rpm per bit
 */
#define GET_PID_RPM(RPM) (uint16_t)(RPM * 4)

/**
 * @brief Service 01 PID 1C - OBD standards this vehicle conforms to
 */
#define OBD_STANDARS 0x1C

/**
 * World Wide Harmonized OBD (WWH OBD)
 */
#define OBD_STANDARS_RESPONSE 21

/**
 * @brief Fuel Type Coding
 */
#define FUEL_TYPE 0x51

/**
 * Gasoline
 */
#define FUEL_TYPE_RESPONSE 1

#endif