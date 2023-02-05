#include <deque>
#include <string>

#include "aliases/memory.hpp"
#include "commands.hpp"
#include "commands_definition.hpp"
#include "defines.h"
#include "variables.h"
#include "webserial/handlers/handlers.hpp"

using namespace web_serial;

bool web_serial::paired = false;
table_data web_serial::in_table;
std::deque<serial_command> web_serial::pending_commands;
std::deque<serial_command> web_serial::output_commands;

void web_serial::setup() {
  web_serial::in_table.reserve(17);
}

void web_serial::command_handler() {
  if (pending_commands.size()) {
    serial_command command = pending_commands.front();
    uint8_t payload[123];
    
    table_data out_table;

    std::fill_n(payload, 123, 0x0);
    
    switch(command.command & 0xF0) {
      case WS_COMMAND_TYPE::CORE:
        handlers::handle_core(command);
        break;
      case WS_COMMAND_TYPE::TABLES:
        handlers::handle_tables(command);
        break;
      case WS_COMMAND_TYPE::EFI_CONFIG:
        handlers::handle_efi_config(command);
        break;
      case WS_COMMAND_TYPE::FW:
        handlers::handle_fw(command);
        break;
      default:
        web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::GENERIC_UNKNOWN_CMD, payload);
    }
    
    pending_commands.pop_front();
  }
}

void web_serial::send_deque() {
  if (output_commands.size()) {
    serial_command out_comm = output_commands.front();
    output_commands.pop_front();
    uint8_t serialized_command[128];
    export_command(out_comm, serialized_command);
    CDC_Transmit_FS(serialized_command, 128);
    // HAL_Delay(10);
  }
}

void web_serial::queue_command(serial_command command) { pending_commands.push_back(command); }

void web_serial::send_command(serial_command command) { output_commands.push_back(command); }

void web_serial::reply_command(uint8_t command, uint8_t status, uint8_t *payload) {
  uint8_t serialized_command[128];
  serial_command out_comm = create_command(command, status, payload);
  export_command(out_comm, serialized_command);
  CDC_Transmit_FS(serialized_command, 128);
}

void web_serial::queue_reply_command(uint8_t command, uint8_t status, uint8_t *payload) {
  serial_command out_comm = create_command(command, status, payload);
  output_commands.push_back(out_comm);
}