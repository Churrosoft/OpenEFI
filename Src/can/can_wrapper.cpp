#include "can_wrapper.h"

uint32_t TxMailboxStatus = 0;

// ISO:TP Flags
bool is_iso_tp_control_flow_frame = false;
uint8_t iso_tp_flow_status = -1;
uint8_t iso_tp_block_size = -1;
uint8_t iso_tp_separation_time = -1;

CAN_HandleTypeDef CanHandle;
CAN_TxHeaderTypeDef TxHeader;

bool transmiting_frame = false;

typedef struct {
  uint32_t stdId;
  uint32_t extId;
  uint32_t frame_size; // unused
  bool is_extended_frame;
  std::deque<std::deque<uint8_t>> frame_data;
} pending_can_message;

std::deque<pending_can_message> TxMailbox;
std::deque<pending_can_message> RxMailbox;

// podrrrriiaaa ser un solo deque sin anidar, pero complejiza un poco el on_loop
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
    for (uint8_t i = 0; i < frame.size(); i++) can_data[i] = frame.at(i);

    CAN_TxHeaderTypeDef CanTxHeader;
    CanTxHeader.DLC = frame.size();

    HAL_CAN_AddTxMessage(&hcan1, &CanTxHeader, can_data, &TxMailboxStatus);
  }

  // transmission complete, reset iso_tp flags
  if (transmiting_frame && !pending_frame_data.size()) {
    transmiting_frame = false;
    iso_tp_flow_status = -1;
    iso_tp_block_size = -1;
    iso_tp_separation_time = -1;
  }

  // check transmission status, queue new frame:
  if (!transmiting_frame && TxMailbox.size() &&
      HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {
    pending_can_message tx = TxMailbox.front();
    TxMailbox.pop_front();

    if (!tx.is_extended_frame) {
      // mensaje iso tp de un unico frame:
      CAN_TxHeaderTypeDef CanTxHeader;
      CanTxHeader.StdId = tx.stdId;
      CanTxHeader.ExtId = tx.extId;

      std::deque<uint8_t> frame = tx.frame_data.at(0);
      uint8_t can_data[8];
      for (uint8_t i = 1; i < frame.size(); i++) can_data[i] = frame.at(i - 1);
      CLEAR_BIT(can_data[0], 0);
      CanTxHeader.DLC = frame.size();
      HAL_CAN_AddTxMessage(&hcan1, &CanTxHeader, can_data, &TxMailboxStatus);

    } else {
      CAN_TxHeaderTypeDef CanTxHeader;
      CanTxHeader.StdId = tx.stdId;
      CanTxHeader.ExtId = tx.extId;

      std::deque<uint8_t> frame = tx.frame_data.at(0);
      uint8_t can_data[8];
      for (uint8_t i = 2; i < frame.size(); i++) can_data[i] = frame.at(i - 2);

      // mark first byte as ISO "FF Frame"
      uint16_t frame_size = tx.frame_data.size();

      Nibbler iso_tp_d0;
      iso_tp_d0.nibbles.first = 0;
      SET_BIT(iso_tp_d0.nibbles.first, 0);
      iso_tp_d0.nibbles.second = (uint8_t)(frame_size >> 8) & 0xFF;

      can_data[0] = iso_tp_d0.byte_value;
      can_data[1] = (uint8_t)frame_size;

      CanTxHeader.DLC = frame.size();
      HAL_CAN_AddTxMessage(&hcan1, &CanTxHeader, can_data, &TxMailboxStatus);

      std::swap(tx.frame_data, pending_frame_data);
    }
  }
}

void CAN::on_message(uint32_t stdId, uint32_t extId, uint8_t *payload,
                     uint8_t payload_size) {
  // TODO: garrar la pala
}