#include "../commands.hpp"
#include "defines.h"
#include "handlers.hpp"
#include "ignition/include/ignition.hpp"
#include "memory/include/tables.hpp"

void handlers::handle_tables(web_serial::serial_command command) {
  uint8_t payload[123];
  
  table_ref table;
  uint16_t selected_table;
  table_data out_table;
  
  switch (command.command) {
    case WS_COMMAND::TABLES_GET_METADATA: {
      // esto tiene que devolver el X/Y maximo de la tabla seleccionada
      selected_table = ((uint16_t)command.payload[0] << 8) + command.payload[1];
      
      switch (selected_table) {
        case TABLES_IGNITION_TPS:
          table = TABLES_IGNITION_TPS_SETTINGS;
          break;

        default:
          web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::TABLES_INVALID_TABLE, payload);
          return;
      }

      // export table config
      payload[0] = (table.x_max >> 8) & 0xFF;
      payload[1] = table.x_max & 0xFF;

      payload[2] = (table.y_max >> 8) & 0xFF;
      payload[3] = table.y_max & 0xFF;
    
      web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_OK, payload);
      break;
    }
    case WS_COMMAND::TABLES_GET: {
      // el manejo de tabla es cuasi el mismo que en get_metadata
      selected_table = ((uint16_t)command.payload[0] << 8) + command.payload[1];
      
      switch (selected_table) {
        case TABLES_IGNITION_TPS: {
          table = TABLES_IGNITION_TPS_SETTINGS;

          // aca esta el caso de que haya una tabla leida, pero con falla'
          if (/* !ignition::loaded && */ ignition::error) {
            web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::TABLES_CRC_ERROR, payload);
            trace_printf("WEBUSB TABLE CRC ERROR");
            return;
          }

          /* tabla sin leer pero con falla */
          if (!ignition::loaded) {
            out_table = tables::read_all(table);
            if (!tables::validate(table, out_table)) {
              web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::TABLES_CRC_ERROR, payload);
              trace_printf("WEBUSB TABLE CRC ERROR");
              return;
            }
          } else {
            // crrrreeoo que con copiar refe y no valor estaria
            out_table = ignition::avc_tps_rpm;
          }

          break;
        }
        case TABLES_INJECTION_VE_MAIN: {
          table = TABLES_INJECTION_VE_SETTINGS;

          out_table = tables::read_all(table);
          if (!tables::validate(table, out_table)) {
            web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::TABLES_CRC_ERROR, payload);
            trace_printf("WEBUSB TABLE CRC ERROR [TABLES_INJECTION_VE_MAIN]");
            return;
          }

          break;
        }
      }

      /* tables::plot_table(out_table); */
      uint8_t table_index = 0;
      for (auto table_row : out_table) {
        tables::dump_row(table_row, payload);
        // FIXME: para evitar que openefi tuner reviente el comando, solo reviso el checksum alla al borrar un
        // comando
        payload[120] = table_index;
        table_index++;
        
        web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_OK, payload);
        /*  web_serial::send_deque(); */
      }

      web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_OK & 0x01, payload);

      break;
    }
    case WS_COMMAND::TABLES_PUT: {
      if (!command.is_valid) {
        web_serial::pending_commands.pop_front();
        web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::TABLES_CRC_ERROR, payload);
        trace_printf("WEBUSB TABLES_PUT CRC ERROR ");
        return;
      }
      
      uint8_t table_index = command.payload[0];
      uint8_t table_row_size = command.payload[1];

      std::vector<int32_t> row = tables::put_row(command.payload, table_row_size);

      web_serial::in_table.insert(web_serial::in_table.begin() + table_index, row);
      break;
    }

    case WS_COMMAND::TABLES_WRITE: {
      // TODO: write only changed rows

      selected_table = ((uint16_t)command.payload[0] << 8) + command.payload[1];
      uint32_t table_crc =
          (uint32_t)(command.payload[3] << 8) + (command.payload[4] << 16) + (command.payload[5] << 24) + command.payload[2];

      switch (selected_table) {
        case TABLES_IGNITION_TPS:
          table = TABLES_IGNITION_TPS_SETTINGS;
        case TABLES_INJECTION_VE_MAIN:
          table = TABLES_INJECTION_VE_SETTINGS;
          break;

        default: {
          web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::TABLES_WRITE_FAIL, payload);
          return;
        }
      }

      if (!tables::validate(table, web_serial::in_table, table_crc)) {
        for (auto ti : web_serial::in_table) {
          ti.clear();
        }
        web_serial::in_table.clear();
        web_serial::pending_commands.pop_front();

        web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::TABLES_WRITE_FAIL, payload);
        return;
      }

      tables::update_table(web_serial::in_table, table);
      // despues de updatear patcheamos la tabla en ram:

      switch (selected_table) {
        case TABLES_IGNITION_TPS:
          ignition::avc_tps_rpm = web_serial::in_table;
          for (auto old_rows : ignition::avc_tps_rpm) {
            old_rows.clear();
          }
          for (auto new_rows : web_serial::in_table) {
            ignition::avc_tps_rpm.push_back(new_rows);
          }
          break;
      }

      for (auto ti : web_serial::in_table) {
        ti.clear();
      }
      web_serial::in_table.clear();

      web_serial::reply_command(command.command, WS_COMMAND_STATUS::CMD_OK, payload);
      break;
    }
    default:
      web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::GENERIC_UNKNOWN_CMD, payload);
  }
}