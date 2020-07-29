#ifndef SENSORS_CPP
#define SENSORS_CPP
#include "../../qfplib/qfplib-m3.h"
#include "./input_handler.hpp"
#include "defines.h"
//sensores:
#include "./include/tps.hpp"
#include "./include/temp.hpp"
#include "./include/iat.hpp"
#include "./include/map.hpp"

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

  //Struct con info de todos los sensores
  struct SensorValues
  {
    uint8_t _TPS;
    uint16_t _MAP;
    uint16_t TEMP;
    uint16_t IAT;
  };
  typedef struct SensorValues SensorValues;

  extern SensorValues values;

} // namespace sensors

#endif