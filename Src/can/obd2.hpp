// PONELE QUE ENTENDI ALGO DE TANTO PDF:
// codigo para soportar OBD-2 para scanners genericos siguiendo estandar:
// Layer 7 SAE J1979/ISO 15031-5
// Layer 5/6 ISO 15765-4:2021
// Layer 2 ISO 15765-2:2016
// Layer 1-2 ISO 9141-2

#include "can_enviroment.h"
#include "dtc_codes.h"
#include "obd2_pid.h"
#include "obd2_sid.h"
#include "obd2_status.h"

#include <cstring>
#include <deque>

typedef struct {
  uint8_t command;
  // TODO: deque en vez de array fijo
  uint8_t payload[7];
} can_message;

std::deque<can_message> pending_can_messages;
#define OBD2_CAN_ADDR_RESPONSE 0x7E8

namespace OBD2 {

void write_dtc_to_mem();
void parse_can_message();
void queue_can_message();

void loop(void);
namespace {
  static inline void send_can_message(uint8_t *data, uint8_t data_size) {
    TxHeader.StdId = OBD2_CAN_ADDR_RESPONSE;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.DLC = data_size;
    TxHeader.TransmitGlobalTime = DISABLE;

    /* Request transmission */
    if (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, data, &TxMailbox) !=
        HAL_OK) {
      /* Transmission request Error */
      Error_Handler();
    }

    /* Wait transmission complete */
    while (HAL_CAN_GetTxMailboxesFreeLevel(&CanHandle) != 3) {
    }
  }
} // namespace
} // namespace OBD2
