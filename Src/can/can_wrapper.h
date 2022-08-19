#ifndef CAN_WRAPPER_H
#define CAN_WRAPPER_H

#include <deque>

#include "can_enviroment.h"
#include "defines.h"
#include "can.h"

// segun la santa wikipedia porque no lo encontre en ISO 15031-5/6
#define OBD2_CAN_ADDR 0x7DF
#define OBD2_CAN_ADDR_RESPONSE 0x7E8

// TODO: que esto quede solo para ver a que modulo patear los mensajes que
// llegan

namespace CAN {
void send_data_to_scanner();

void update_dashdash();

uint8_t check_crc(); // en teoriiiaa puede revisarlo por su cuenta HAL y
                     // descartar los invalidos

void on_setup();
void on_loop();
void on_message(uint32_t, uint32_t, uint8_t *, uint8_t);

} // namespace CAN

#endif