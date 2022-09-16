
// archivo para tirar los mailboxes de can
#include "main.h"
#include <deque>

#ifndef CAN_ENVIROMENT_H
#define CAN_ENVIROMENT_H

extern CAN_HandleTypeDef CanHandle;
extern uint32_t TxMailboxStatus;
extern CAN_TxHeaderTypeDef TxHeader;

// ISO:TP Flags
extern bool is_iso_tp_control_flow_frame;
extern bool iso_tp_waiting_cfc;
// extern uint8_t iso_tp_flow_status;
// extern uint8_t iso_tp_block_size;
extern int8_t iso_tp_output_block_size;
extern int32_t iso_tp_separation_time;
extern int32_t iso_tp_separation_time_start_time;
extern uint32_t iso_tp_cfc_index;

typedef struct {
  uint32_t stdId;
  uint32_t extId;
  uint32_t frame_size; // unused
  uint32_t current_transfered;
  bool is_extended_frame;
  std::deque<std::deque<uint8_t>> frame_data;
} can_message;

extern std::deque<can_message> TxMailbox;
extern std::deque<can_message> RxMailbox;

#endif