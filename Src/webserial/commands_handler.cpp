#include <cstring>
#include <deque>
#include <string>
#include <vector>

#include "../../lib/json/include/nlohmann/json.hpp"
#include "../ignition/include/ignition.hpp"
#include "../memory/include/config.hpp"
#include "../sensors/sensors.hpp"
#include "aliases/memory.hpp"
#include "commands.hpp"
#include "commands_definition.hpp"
#include "defines.h"
#include "engine_status.hpp"
#include "variables.h"

using json = nlohmann::json;
using namespace web_serial;

void to_json(json& j, const engine_config& p) {
  j = json{
      {"ready", p.ready},
      {"Injection",
       {{"targetLambda", p.Injection.targetLambda},
        {"targetStoich", p.Injection.targetStoich},
        {"enable_alphaN", p.Injection.enable_alphaN},
        {"enable_speedDensity", p.Injection.enable_speedDensity},

        {"injector",
         {
             {"flowCCMin", p.Injection.injector.flowCCMin},
             {"injectorCount", p.Injection.injector.injectorCount},
             {"fuelPressure", p.Injection.injector.fuelPressure},
             {"fuelDensity", p.Injection.injector.fuelDensity},
             {"onTime", p.Injection.injector.onTime},
             {"offTime", p.Injection.injector.offTime},
         }},

        {"alphaN_ve_table",
         {{"x_max", p.Injection.alphaN_ve_table.x_max},
          {"y_max", p.Injection.alphaN_ve_table.y_max},
          {"memory_address", p.Injection.alphaN_ve_table.memory_address}

         }}}},
  };
}

std::deque<serial_command> pending_commands;
std::deque<serial_command> output_commands;

table_data in_table;
bool table_lock;
bool web_serial::paired = false;

void web_serial::setup() { in_table.reserve(17); }

void web_serial::command_handler() {
  if (pending_commands.size()) {
    serial_command command = pending_commands.front();
    serial_command out_comm;
    uint8_t payload[123];
    uint8_t serialized_command[128];
    table_ref table;
    uint16_t selected_table;
    table_data out_table;

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
        web_serial::paired = true;
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
        // int mockrpm = 750 + (rand() % 5750);
        int mockrpm = _RPM;
        int mocktemp = 1 + (rand() % 13000);
        int mockload = 1 + (rand() % 100);
        int mockbattery = 1 + (rand() % 1500);

        payload[0] = (mockrpm >> 8) & 0xFF;
        payload[1] = mockrpm & 0xFF;

        // legacy sensor status:
        payload[2] = (mocktemp >> 8) & 0xFF;
        payload[3] = mocktemp & 0xFF;

        payload[4] = (mockload >> 8) & 0xFF;
        payload[5] = mockload & 0xFF;

        payload[6] = (mockbattery >> 8) & 0xFF;
        payload[7] = mockbattery & 0xFF;

        payload[8] = (_AE >> 8) & 0xFF;
        payload[9] = _AE & 0xFF;

        // efi status
        payload[10] = 1;

        // estado / valores sensores;
        // -1 == sensor desconectado / no disponible

        // TPS simple/doble
        payload[11] = (uint8_t)mockload;
        payload[12] = (uint8_t)(mockload >> 8) & 0xFF;
        payload[13] = (uint8_t)(mockload >> 16) & 0xFF;
        payload[14] = (uint8_t)(mockload >> 24) & 0xFF;

        // MAP
        payload[15] = (uint8_t)sensors::values._MAP;
        payload[16] = (uint8_t)(sensors::values._MAP >> 8) & 0xFF;
        payload[17] = (uint8_t)(sensors::values._MAP >> 16) & 0xFF;
        payload[18] = (uint8_t)(sensors::values._MAP >> 24) & 0xFF;

        // TEMP
        payload[19] = (uint8_t)mocktemp;
        payload[20] = (uint8_t)(mocktemp >> 8) & 0xFF;
        payload[21] = (uint8_t)(mocktemp >> 16) & 0xFF;
        payload[22] = (uint8_t)(mocktemp >> 24) & 0xFF;

        // IAT
        payload[23] = (uint8_t)sensors::values.IAT;
        payload[24] = (uint8_t)(sensors::values.IAT >> 8) & 0xFF;
        payload[25] = (uint8_t)(sensors::values.IAT >> 16) & 0xFF;
        payload[26] = (uint8_t)(sensors::values.IAT >> 24) & 0xFF;

        // BATT
        payload[27] = (uint8_t)mockbattery;
        payload[28] = (uint8_t)(mockbattery >> 8) & 0xFF;
        payload[29] = (uint8_t)(mockbattery >> 16) & 0xFF;
        payload[30] = (uint8_t)(mockbattery >> 24) & 0xFF;

        // LMB
        payload[31] = (uint8_t)sensors::values.LMB;
        payload[32] = (uint8_t)(sensors::values.LMB >> 8) & 0xFF;
        payload[33] = (uint8_t)(sensors::values.LMB >> 16) & 0xFF;
        payload[34] = (uint8_t)(sensors::values.LMB >> 24) & 0xFF;

        // Ignition Status (20b)
        payload[60] = (uint8_t)_AE;
        payload[61] = (uint8_t)(_AE >> 8) & 0xFF;
        payload[62] = (uint8_t)(_AE >> 16) & 0xFF;
        payload[63] = (uint8_t)(_AE >> 24) & 0xFF;
        payload[80] = 1;

        // Injection Status (20b)
        payload[80] = (uint8_t)_INY_T1;
        payload[81] = (uint8_t)(_INY_T1 >> 8) & 0xFF;
        payload[82] = (uint8_t)(_INY_T1 >> 16) & 0xFF;
        payload[83] = (uint8_t)(_INY_T1 >> 24) & 0xFF;

        payload[84] = (uint8_t)_INY_T1;
        payload[85] = (uint8_t)(_INY_T1 >> 8) & 0xFF;
        payload[86] = (uint8_t)(_INY_T1 >> 16) & 0xFF;
        payload[87] = (uint8_t)(_INY_T1 >> 24) & 0xFF;

        payload[88] = (uint8_t)(int32_t)efi_status.injection.targetAFR;
        payload[89] = (uint8_t)((int32_t)efi_status.injection.targetAFR >> 8) & 0xFF;
        payload[90] = (uint8_t)((int32_t)efi_status.injection.targetAFR >> 16) & 0xFF;
        payload[91] = (uint8_t)((int32_t)efi_status.injection.targetAFR >> 24) & 0xFF;

        payload[100] = 1;

        out_comm = create_command(CORE_STATUS, payload);
        export_command(out_comm, serialized_command);
        CDC_Transmit_FS(serialized_command, 128);
        break;
      }

      case EFI_CONFIG_GET: {
        auto eficfg = efi_cfg::get();
        json efi_json{eficfg};
        auto json_string = efi_json.dump();
        std::vector<std::string> output;

        for (uint16_t i = 0; i < json_string.length(); i += 100) {
          auto output_text = json_string.substr(i, 100);
          memcpy(payload, output_text.c_str(), output_text.length());
          out_comm = create_command(EFI_CONFIG_CHUNK, payload);
          export_command(out_comm, serialized_command);
          output_commands.push_back(out_comm);
          std::fill_n(payload, 123, 0x0);
        }

        payload[0] = (uint8_t)json_string.length();
        payload[1] = (uint8_t)(json_string.length() >> 8) & 0xFF;
        payload[2] = (uint8_t)(json_string.length() >> 16) & 0xFF;
        payload[3] = (uint8_t)(json_string.length() >> 24) & 0xFF;

        out_comm = create_command(EFI_CONFIG_END, payload);
        export_command(out_comm, serialized_command);
        output_commands.push_back(out_comm);
        break;
      }

      case EFI_CONFIG_WRITE: {
      }
      case EFI_CONFIG_RESET: {
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

            // aca esta el caso de que haya una tabla leida, pero con falla'
            if (/* !ignition::loaded && */ ignition::error) {
              out_comm = create_command(TABLES_CRC_ERROR, payload);
              pending_commands.pop_front();
              output_commands.push_back(out_comm);
              trace_printf("WEBUSB TABLE CRC ERROR");
              return;
            }

            /* tabla sin leer pero con falla */
            if (!ignition::loaded) {
              out_table = tables::read_all(table);
              if (tables::validate(table, out_table)) {
                pending_commands.pop_front();
                output_commands.push_back(out_comm);
                trace_printf("WEBUSB TABLE CRC ERROR");
                return;
              }
            } else {
              // crrrreeoo que con copiar refe y no valor estaria
              out_table = ignition::avc_tps_rpm;
            }

            break;
          }
        }

        tables::plot_table(out_table);
        uint8_t table_index = 0;
        for (auto table_row : out_table) {
          tables::dump_row(table_row, payload);
          // FIXME: para evitar que openefi tuner reviente el comando, solo reviso el checksum alla al borrar un
          // comando
          payload[120] = table_index;
          table_index++;
          out_comm = create_command(TABLES_DATA_CHUNK, payload);
          output_commands.push_back(out_comm);
          /*  web_serial::send_deque(); */
        }

        out_comm = create_command(TABLES_DATA_END_CHUNK, payload);
        output_commands.push_back(out_comm);

        break;
      }

      case TABLES_PUT: {
        if (!command.is_valid) {
          pending_commands.pop_front();
          out_comm = create_command(TABLES_CRC_ERROR, payload);
          output_commands.push_back(out_comm);
          trace_printf("WEBUSB TABLES_PUT CRC ERROR ");
          return;
        }

        uint8_t table_index = command.payload[0];
        uint8_t table_row_size = command.payload[1];

        std::vector<int32_t> row = tables::put_row(command.payload, table_row_size);

        in_table.insert(in_table.begin() + table_index, row);
        break;
      }

      case TABLES_WRITE: {
        // TODO: write only changed rows

        selected_table = ((uint16_t)command.payload[0] << 8) + command.payload[1];
        uint32_t table_crc =
            (uint32_t)(command.payload[3] << 8) + (command.payload[4] << 16) + (command.payload[5] << 24) + command.payload[2];

        switch (selected_table) {
          case TABLES_IGNITION_TPS:
            table = TABLES_IGNITION_TPS_SETTINGS;
            break;
        }

        if (!tables::validate(table, in_table, table_crc)) {
          for (auto ti : in_table) {
            ti.clear();
          }
          in_table.clear();
          pending_commands.pop_front();

          out_comm = create_command(TABLES_WRITE_FAIL, payload);
          export_command(out_comm, serialized_command);
          output_commands.push_back(out_comm);

          return;
        }

        tables::update_table(in_table, table);
        // despues de updatear patcheamos la tabla en ram:

        switch (selected_table) {
          case TABLES_IGNITION_TPS:
            ignition::avc_tps_rpm = in_table;
            for (auto old_rows : ignition::avc_tps_rpm) {
              old_rows.clear();
            }
            for (auto new_rows : in_table) {
              ignition::avc_tps_rpm.push_back(new_rows);
            }
            break;
        }

        for (auto ti : in_table) {
          ti.clear();
        }
        in_table.clear();

        out_comm = create_command(TABLES_WRITE_OK, payload);
        export_command(out_comm, serialized_command);
        CDC_Transmit_FS(serialized_command, 128);

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
    // HAL_Delay(10);
  }
}

void web_serial::queue_command(serial_command command) { pending_commands.push_back(command); }

void web_serial::send_command(serial_command command) { output_commands.push_back(command); }
