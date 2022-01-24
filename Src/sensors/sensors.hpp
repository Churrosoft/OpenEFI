#ifndef SENSORS_CPP
#define SENSORS_CPP

#include "./input_handler.hpp"
#include "defines.h"
#include "user_defines.h"

//sensores:
#include "./include/tps.hpp"
#include "./include/temp.hpp"
#include "./include/iat.hpp"
#include "./include/map.hpp"
#include "./input_handler.hpp"

#include <stdint.h>

namespace sensors
{
  /** @brief Setup inicial de todos los sensores
    */
  void setup(void);

  /** @brief Loop principal de los sensores, corre cada 75mS */
  void loop(void);
  /** @brief Loop secundario, corre cada 250mS */
  void loop_low_priority(void);

  // Struct con info de todos los sensores
  typedef struct 
  {
    int32_t _TPS;
    int32_t _MAP;
    int32_t TEMP;
    int32_t IAT;
  }SensorValues;
  // typedef struct SensorValues SensorValues;

  extern SensorValues values;

} // namespace sensors

#endif