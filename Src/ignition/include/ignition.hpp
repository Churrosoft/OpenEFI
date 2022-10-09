

#ifndef IGNITION_HPP
#define IGNITION_HPP

#include <limits.h>
#include <stdint.h>

#include <vector>

#include "aliases/memory.hpp"
#include "aliases/sensors.hpp"
#include "variables.h"

/**
 *  @addtogroup Ignition
 *  @brief Namespace con logica relacionada al encendido
 * @{
 */
namespace ignition {
  void interrupt();
  void setup();
  void set_fixed_advance(int32_t);
  void disable_fixed_advance();

  // tabla de avance, por TPS y RPM
  extern table_data avc_tps_rpm;
  extern int16_t avc_rpm[13];
  extern int16_t avc_tps[13];
  extern bool loaded;
  extern bool fixed_mode;
  extern bool error;

}    // namespace ignition

/*! @} End of Doxygen Ignition*/

#endif