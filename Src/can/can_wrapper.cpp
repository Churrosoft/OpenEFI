#include "can_wrapper.h"

uint32_t TxMailboxStatus = 0;

// ISO:TP Flags
bool is_iso_tp_control_flow_frame = false;
uint8_t iso_tp_flow_status = -1;
uint8_t iso_tp_block_size = -1;
uint8_t iso_tp_separation_time = -1;

bool transmiting_frame = false;

std::deque<can_message> TxMailbox;
std::deque<can_message> RxMailbox;
can_message RxBufferMessage;

// podrrrriiaaa ser un solo deque sin anidar, pero complejiza un poco el on_loop
std::deque<std::deque<uint8_t>> pending_frame_data;

// "on_message" agrega un flag para controlar cuando cae un control flow frame
// este loop se encarga de revisar el mismo e ir enviando los mensajes
// pendientes
void CAN::on_loop() {

  if (transmiting_frame && iso_tp_separation_time == -1 &&
      is_iso_tp_control_flow_frame && pending_frame_data.size() &&
      HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {

    std::deque<uint8_t> frame = pending_frame_data.front();

    uint8_t can_data[8];
    for (uint8_t i = 0; i < frame.size(); i++) can_data[i] = frame.at(i);

    CAN_TxHeaderTypeDef CanTxHeader;
    CanTxHeader.DLC = frame.size();

    auto txResult =
        HAL_CAN_AddTxMessage(&hcan1, &CanTxHeader, can_data, &TxMailboxStatus);

    if (txResult == HAL_OK) {
      pending_frame_data.pop_front();
    }
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
    can_message tx = TxMailbox.front();

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

      auto txResult = HAL_CAN_AddTxMessage(&hcan1, &CanTxHeader, can_data,
                                           &TxMailboxStatus);

      if (txResult == HAL_OK) {
        TxMailbox.pop_front();
      }
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

      auto txResult = HAL_CAN_AddTxMessage(&hcan1, &CanTxHeader, can_data,
                                           &TxMailboxStatus);

      if (txResult == HAL_OK) {
        std::swap(tx.frame_data, pending_frame_data);
        TxMailbox.pop_front();
      }
    }
  }
}

void CAN::on_message(uint32_t stdId, uint32_t extId, uint8_t *payload,
                     uint8_t payload_size) {
  // TODO: garrar la pala
  // Puede llegar el caso que llegue un frame FF/FC tengo que agregar a
  // RxBufferMessage esa data a medida que llegan el resto de paquetes y luego
  // moverlo al mailbox

  RxBufferMessage.stdId = stdId;
  RxBufferMessage.extId = extId;

  // check if is single frame/ multiframe
  Nibbler iso_tp_header;
  iso_tp_header.byte_value = payload[0];

  // single frame
  if (iso_tp_header.nibbles.first == 0) {
    std::deque<uint8_t> frame;
    for (uint8_t i = 0; i < iso_tp_header.nibbles.second; i++) {
      frame.push_back(payload[i + 1]);
    }
    RxBufferMessage.frame_data.push_back(frame);
    RxMailbox.push_back(RxBufferMessage);
  }

  // multiple frame (first frame)
  if (iso_tp_header.nibbles.first == 1) {
    iso_tp_block_size = payload[1];
    // inverted (big endian => litte endian)
    int16_t iso_tp_data_size = (uint16_t)payload[3] + payload[2] << 8;
    RxBufferMessage.frame_size = iso_tp_block_size;
    // TODO: aca toca mandar un Flow Control para setear block size/tiempo
  }

  // multiple frame (consecutive frame)
  if (iso_tp_header.nibbles.first == 2) {
    uint32_t iso_tp_cf_index = iso_tp_header.nibbles.second;
    std::deque<uint8_t> frame;

    for (uint8_t i = 0; i < iso_tp_cf_index; i++) {
      frame.push_back(payload[i + 1]);
    }

    RxBufferMessage.frame_data.assign(iso_tp_cf_index, frame);

    RxBufferMessage.current_transfered++;
  }

  // push message to mailbox && clear buffer
  if (RxBufferMessage.current_transfered <= RxBufferMessage.frame_size) {
    RxMailbox.push_back(RxBufferMessage);
    for (auto frame : RxBufferMessage.frame_data) {
      frame.clear();
    }

    RxBufferMessage.frame_data.clear();

    RxBufferMessage = can_message();
  }

  // multiple frame (flow control)
  if (iso_tp_header.nibbles.first == 3) {
  }
}