#ifndef INJECTION_HPP
#define INJECTION_HPP

#include <stdint.h>

#include <vector>

#include "aliases/memory.hpp"
#include "aliases/sensors.hpp"
#include "custom_vars.hpp"
#include "variables.h"

/**
 * @addtogroup Injection
 * @brief Namespace con logica de inyeccion,
 *  desde aca se selecciona el algoritmo a utilizar
 * @{
 */
namespace injection {
  void setup();
  void on_loop();

  /**
   * @addtogroup Injection
   * @brief Speed-N calculations
   * @{
   */
  namespace speedN {

    // calcular tiempo base
    fuel_mass_t calculate_injection_fuel();

    // lambda
    mix_mass_t calculate_correction_time();

    // estas son internas:
    air_mass_t get_airmass();

  }    // namespace speedN
  /*! @} End of Doxygen speedN*/

  /**
   * @addtogroup Injection
   * @brief warm up enrichment (WUE) calculations
   * @{
   */
  namespace WUE {
    // sacado de la tabla en eeprom
    int32_t get_time();
  }    // namespace WUE
       /*! @} End of Doxygen WUE*/

  /**
   * @addtogroup Injection
   * @brief Injectors calculations
   * @{
   */
  namespace Injectors {
    int32_t get_base_time();
    int32_t get_off_time();
    int32_t get_battery_correction();
    int32_t get_pressure_correction();
    int32_t get_wall_wetting_correction();
    int32_t fuel_mass_to_time(fuel_mass_t);
    void set_injectorFlow();

  }    // namespace Injectors
  /*! @} End of Doxygen Injectors*/

  /**
   * @addtogroup Injection
   * @brief Acceleration (a lo bomba de [nelson] pique[t] )
   * @{
   */
  namespace Acceleration {
    int32_t get_acc_from_tps_map();
  }    // namespace Acceleration
  /*! @} End of Doxygen Acceleration*/

}    // namespace injection

/*! @} End of Doxygen injection*/

#endif