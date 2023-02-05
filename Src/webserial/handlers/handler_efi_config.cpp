#include "../commands.hpp"
#include "defines.h"
#include "engine_status.hpp"
#include "handlers.hpp"
#include "memory/include/config.hpp"
#include "sensors/sensors.hpp"

#include "../../lib/json/include/nlohmann/json.hpp"
#include "webserial/handlers/handlers.hpp"

using json = nlohmann::json;

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

void handlers::handle_efi_config(web_serial::serial_command command) {
  uint8_t payload[123];
  
  switch (command.command) {
    case WS_COMMAND::EFI_CONFIG_GET: {
      auto eficfg = efi_cfg::get();
      json efi_json{eficfg};
      auto json_string = efi_json.dump();
      std::vector<std::string> output;
      
      for (uint16_t i = 0; i < json_string.length(); i += 100) {
        auto output_text = json_string.substr(i, 100);
        memcpy(payload, output_text.c_str(), output_text.length());
        
        web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_OK, payload);
        
        std::fill_n(payload, 123, 0x0);
      }

      payload[0] = (uint8_t)json_string.length();
      payload[1] = (uint8_t)(json_string.length() >> 8) & 0xFF;
      payload[2] = (uint8_t)(json_string.length() >> 16) & 0xFF;
      payload[3] = (uint8_t)(json_string.length() >> 24) & 0xFF;

      web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_OK & 0x01, payload);
      break;
    }

    case WS_COMMAND::EFI_CONFIG_WRITE: {
      // TODO
      break;
    }
    case WS_COMMAND::EFI_CONFIG_RESET: {
      // TODO
      break;
    }
    default:
      web_serial::queue_reply_command(command.command, WS_COMMAND_STATUS::CMD_ERR & WS_COMMAND_ERROR::GENERIC_UNKNOWN_CMD, payload);

  }
}