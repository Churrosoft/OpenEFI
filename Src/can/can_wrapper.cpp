#include "can_wrapper.h"

uint32_t TxMailbox = 0;

// ISO:TP Flags
bool is_iso_tp_control_flow_frame = false;
uint8_t iso_tp_flow_status = -1;
uint8_t iso_tp_block_size = -1;
uint8_t iso_tp_separation_time = -1;

CAN_HandleTypeDef CanHandle;
CAN_TxHeaderTypeDef TxHeader;

bool transmiting_frame = false;
std::deque<std::deque<uint8_t>> pending_frame_data;

// "on_message" agrega un flag para controlar cuando cae un control flow frame
// este loop se encarga de revisar el mismo y ir enviando los mensajes
// pendientes
void CAN::on_loop() {

  if (transmiting_frame && iso_tp_separation_time == -1 &&
      is_iso_tp_control_flow_frame && pending_frame_data.size() &&
      HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {

    std::deque<uint8_t> frame = pending_frame_data.front();
    pending_frame_data.pop_front();

    uint8_t can_data[8];
    for (uint8_t i = 0; i < frame.size(); i++) can_data[i] = frame[i];

    CAN_TxHeaderTypeDef CanTxHeader;
    CanTxHeader.DLC = frame.size();

    HAL_CAN_AddTxMessage(&hcan1, &CanTxHeader, can_data, &TxMailbox);
  }
}

void CAN::on_message(uint32_t stdId, uint32_t extId, uint8_t *payload,
                     uint8_t payload_size) {
  // TODO: garrar la pala
}