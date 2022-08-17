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
std::deque<uint8_t> peding_frame_data;


// "on_message" agrega un flag para controlar cuando cae un control flow frame
// este loop se encarga de revisar el mismo y ir enviando los mensajes
// pendientes
void CAN::on_loop() {

    if(transmiting_frame && iso_tp_separation_time == -1 && is_iso_tp_control_flow_frame){
        // read byte
    }


}