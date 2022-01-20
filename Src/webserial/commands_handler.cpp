#include "aliases/memory.hpp"
#include "commands.hpp"
#include "commands_definition.hpp"
#include "defines.h"

#include <deque>

using namespace web_serial;

std::deque<serial_command> pending_commands;
std::deque<serial_command> output_commands;

TABLEDATA in_table;
bool table_lock;

void web_serial::setup() { in_table.reserve(17); }

void web_serial::command_handler() {
  if (pending_commands.size()) {
    serial_command command = pending_commands.front();
    serial_command out_comm;
    uint8_t payload[123];
    uint8_t serialized_command[128];
    table_ref table;
    uint16_t selected_table;
    TABLEDATA out_table;

    std::fill_n(payload, 123, 0x0);

    switch (command.command) {

    case CORE_PING: {
      payload[0] = OPENEFI_BOARD_TYPE;

      payload[1] = (OPENEFI_VER_MAJOR >> 8) & 0xFF;
      payload[2] = OPENEFI_VER_MAJOR & 0xFF;

      payload[3] = (OPENEFI_VER_MINOR >> 8) & 0xFF;
      payload[4] = OPENEFI_VER_MINOR & 0xFF;

      payload[5] = (OPENEFI_VER_REV >> 8) & 0xFF;
      payload[6] = OPENEFI_VER_REV & 0xFF;

      out_comm = create_command(CORE_PONG, payload);
      export_command(out_comm, serialized_command);
      CDC_Transmit_FS(serialized_command, 128);
      break;
    }

    case CORE_STATUS_METADA: {

      payload[0] = (MAX_RPM >> 8) & 0xFF;
      payload[1] = MAX_RPM & 0xFF;

      payload[2] = (TEMP_MAX_VALUE >> 8) & 0xFF;
      payload[3] = TEMP_MAX_VALUE & 0xFF;

      out_comm = create_command(CORE_STATUS_METADA, payload);
      export_command(out_comm, serialized_command);
      CDC_Transmit_FS(serialized_command, 128);
      break;
    }

    case CORE_STATUS: {

      int mockrpm = 750 + (rand() % 5750);
      int mocktemp = 1 + (rand() % 130);
      int mockload = 1 + (rand() % 100);
      int mockbattery = 1 + (rand() % 1500);

      payload[0] = (mockrpm >> 8) & 0xFF;
      payload[1] = mockrpm & 0xFF;

      payload[2] = (mocktemp >> 8) & 0xFF;
      payload[3] = mocktemp & 0xFF;

      payload[4] = (mockload >> 8) & 0xFF;
      payload[5] = mockload & 0xFF;

      payload[6] = (mockbattery >> 8) & 0xFF;
      payload[7] = mockbattery & 0xFF;

      out_comm = create_command(CORE_STATUS, payload);
      export_command(out_comm, serialized_command);
      CDC_Transmit_FS(serialized_command, 128);
      break;
    }

    case TABLES_GET_METADATA: {
      // esto tiene que devolver el X/Y maximo de la tabla seleccionada
      selected_table = ((uint16_t)command.payload[0] << 8) + command.payload[1];

      bool errored = false;

      switch (selected_table) {
      case TABLES_IGNITION_TPS:
        table = TABLES_IGNITION_TPS_SETTINGS;
        break;

      default:
        out_comm = create_command(TABLES_INVALID_TABLE, payload);
        export_command(out_comm, serialized_command);
        CDC_Transmit_FS(serialized_command, 128);
        errored = true;
        break;
      }

      if (!errored) {
        // export table config
        payload[0] = (table.x_max >> 8) & 0xFF;
        payload[1] = table.x_max & 0xFF;

        payload[2] = (table.y_max >> 8) & 0xFF;
        payload[3] = table.y_max & 0xFF;

        // mismo comando porque paja uno nuevo
        out_comm = create_command(TABLES_GET_METADATA, payload);
        export_command(out_comm, serialized_command);
        CDC_Transmit_FS(serialized_command, 128);
      }

      break;
    }

    case TABLES_GET: {
      // el manejo de tabla es cuasi el mismo que en get_metadata
      selected_table = ((uint16_t)command.payload[0] << 8) + command.payload[1];

      switch (selected_table) {
      case TABLES_IGNITION_TPS: {
        table = TABLES_IGNITION_TPS_SETTINGS;

        out_table = tables::read_all(table);

        tables::plot_table(out_table);

        for (auto table_row : out_table) {

          tables::dump_row(table_row, payload);
          out_comm = create_command(TABLES_DATA_CHUNK, payload);

          output_commands.push_back(out_comm);
        }
        break;
      }
      }
      out_comm = create_command(TABLES_DATA_END_CHUNK, payload);
      output_commands.push_back(out_comm);

      break;
    }

    case TABLES_PUT: {
      uint8_t table_index = command.payload[0];
      uint8_t table_row_size = command.payload[1];

      std::vector<int32_t> row =
          tables::put_row(command.payload, table_row_size);

      in_table.insert(in_table.begin() + table_index, row);

      break;
    }

    case TABLES_WRITE: {
      tables::plot_table(in_table);

      // TODO: CRC check, move switch to func, write only changed rows

      selected_table = ((uint16_t)command.payload[0] << 8) + command.payload[1];

      switch (selected_table) {
      case TABLES_IGNITION_TPS:
        table = TABLES_IGNITION_TPS_SETTINGS;
        break;
      }
      tables::update_table(in_table, table);

      in_table.clear();

      out_comm = create_command(TABLES_WRITE_OK, payload);
      export_command(out_comm, serialized_command);
      CDC_Transmit_FS(serialized_command, 128);
      BREAKPOINT;
      break;
    }

    default:
      out_comm = create_command(EFI_INVALID_CODE, payload);
      export_command(out_comm, serialized_command);
      CDC_Transmit_FS(serialized_command, 128);
      break;
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
    HAL_Delay(10);
  }
}

void web_serial::queue_command(serial_command command) {
  pending_commands.push_back(command);
}
