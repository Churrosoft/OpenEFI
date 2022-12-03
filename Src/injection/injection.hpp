#ifndef INJECTION_HPP
#define INJECTION_HPP

#include <stdint.h>

#include <vector>

#include "aliases/memory.hpp"
#include "aliases/sensors.hpp"
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
    int32_t calculate_injection_time();

    // lambda
    int32_t calculate_correction_time();

  }    // namespace speedN

}    // namespace injection

/*! @} End of Doxygen injection*/

#endif