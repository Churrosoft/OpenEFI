
// archivo para tirar los mailboxes de can
#include "main.h"
#include <deque>

#ifndef CAN_ENVIROMENT_H
#define CAN_ENVIROMENT_H

extern CAN_HandleTypeDef CanHandle;
extern uint32_t TxMailboxStatus;
extern CAN_TxHeaderTypeDef TxHeader;

extern bool is_iso_tp_control_flow_frame;
extern uint8_t iso_tp_flow_status;
extern uint8_t iso_tp_block_size;
extern uint8_t iso_tp_separation_time;

typedef struct {
  uint32_t stdId;
  uint32_t extId;
  uint32_t frame_size; // unused
  bool is_extended_frame;
  std::deque<std::deque<uint8_t>> frame_data;
} can_message;

extern std::deque<can_message> TxMailbox;

#endif