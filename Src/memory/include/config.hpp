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

/** @addtogroup Memory
 *  @brief Namespace con I/O hacia la memoria SPI externa
 * @{
 */

/** @addtogroup Config
 * @brief datos configurables para OpenEFI
 * @{
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