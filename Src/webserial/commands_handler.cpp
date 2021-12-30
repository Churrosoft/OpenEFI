#include "aliases/memory.hpp"
#include "commands.hpp"
#include "commands_definition.hpp"
#include "defines.h"

#include <vector>

using namespace web_serial;

std::vector<serial_command> pending_commands;

void web_serial::command_handler() {
  if (pending_commands.size()) {
    serial_command command = pending_commands.back();
    serial_command out_comm;
    uint8_t payload[123];
    uint8_t serialized_command[128];
    table_ref table;
    uint16_t selected_table;
    TABLEDATA out_table;

    std::fill_n(payload, 123, 0x0);

    switch (command.command) {
    case TABLES_GET_METADATA: {
      // esto tiene que devolver el X/Y maximo de la tabla seleccionada
      selected_table = ((int16_t)command.payload[0] << 8) + command.payload[1];

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
      selected_table = ((int16_t)command.payload[0] << 8) + command.payload[1];

      switch (selected_table) {
      case TABLES_IGNITION_TPS: {
        table = TABLES_IGNITION_TPS_SETTINGS;

        out_table = tables::read_all(table);

        for (auto table_row : out_table) {

          tables::dump_row(table_row, payload);
          out_comm = create_command(TABLES_DATA_CHUNK, payload);
          export_command(out_comm, serialized_command);
          CDC_Transmit_FS(serialized_command, 128);
        }
        break;
      }
      }
      break;
    }

    default:
      out_comm = create_command(EFI_INVALID_CODE, payload);
      export_command(out_comm, serialized_command);
      CDC_Transmit_FS(serialized_command, 128);
      break;
    }
    trace_printf("CRC 0: %d \n", command.crc[0]);
    trace_printf("CRC 1: %d \n", command.crc[1]);
    pending_commands.pop_back();
  }
}

void web_serial::queue_command(serial_command command) {
  pending_commands.push_back(command);
}
