#ifndef CAN_WRAPPER_H
#define CAN_WRAPPER_H
#include "defines.h"

#define OBD2_CAN_ADDR 0x7DF

namespace CAN {
void send_data_to_scanner();

void update_dashdash();

uint8_t check_crc(); // en teoriiiaa puede revisarlo por su cuenta HAL y
                     // descartar los invalidos

void on_setup();
void on_loop();
void on_message(); // similar a web_serial::command_handler

} // namespace CAN

#endif