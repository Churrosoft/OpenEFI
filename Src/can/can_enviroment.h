
// archivo para tirar los mailboxes de can
#include "main.h"

#ifndef CAN_ENVIROMENT_H
#define CAN_ENVIROMENT_H

extern CAN_HandleTypeDef CanHandle;
extern uint32_t TxMailbox;
extern CAN_TxHeaderTypeDef TxHeader;

#endif