#ifndef MEMORY_CONFIG_HPP
#define MEMORY_CONFIG_HPP

#include "efi_config.hpp"

extern "C" {
#include "w25qxx.h"
}

#include <limits.h>
#include <math.h>
#include <stdint.h>

#include <algorithm>
#include <vector>
#include "../../../lib/json/include/nlohmann/json.hpp"

using json = nlohmann::json;
// https://github.com/Stiffstream/json_dto#how-to-use-it

/** @addtogroup Memory
 *  @brief Namespace con I/O hacia la memoria SPI externa
 * @{
 */

/** @addtogroup Config
 * @brief datos configurables para OpenEFI
 * @{
 */

/* void to_json(json& j, const engine_config& p) {
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
 */
//! Implementacion de tablas 2D con vectores, valores en int32_t
namespace efi_cfg {

  engine_config get();
  void set(engine_config);
  void set_default();

}    // namespace config

/*! @} End of Doxygen Tables*/

/*! @} End of Doxygen Config*/

#endif