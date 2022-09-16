#ifndef CAN_WRAPPER_H
#define CAN_WRAPPER_H

#include "can.h"
#include "can_enviroment.h"
#include "defines.h"
#include "main.h"

// segun la santa wikipedia porque no lo encontre en ISO 15031-5/6
#define OBD2_CAN_ADDR 0x7DF
#define OBD2_CAN_ADDR_RESPONSE 0x7E8

namespace CAN {

/**
 * @brief tarea cronometrada, resvisa si en el bus CAN esta conectado un
 * DashDash y lo actualiza con la info actual de la EFI
 */
void update_dashdash();

/**
 * @brief se encarga de manejar todas las transferencias salientes, se ejecuta
 * con el loop princpial
 *
 */
void on_loop();

/**
 * @brief llamado al recibir un mensaje desde CAN1_RX0_IRQHandler
 * @param uint32_t CAN STD ID
 * @param uint32_t CAN EXT ID
 * @param uint8_t* buffer with payload
 * @param uint8_t CAN DLC (payload size)
 */
void on_message(uint32_t, uint32_t, uint8_t *, uint8_t);

} // namespace CAN

#endif