
// archivo para tirar los mailboxes de can
#include "main.h"

#ifndef CAN_ENVIROMENT_H
#define CAN_ENVIROMENT_H

extern CAN_HandleTypeDef CanHandle;
extern uint32_t TxMailboxStatus;
extern CAN_TxHeaderTypeDef TxHeader;

extern bool is_iso_tp_control_flow_frame;
extern uint8_t iso_tp_flow_status;
extern uint8_t iso_tp_block_size;
extern uint8_t iso_tp_separation_time;

#endif