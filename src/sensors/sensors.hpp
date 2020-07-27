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
  static void setup(void);

  /** @brief Loop principal de los sensores, corre cada 75mS */
  static void loop(void);
  /** @brief Loop secundario, corre cada 250mS */
  static void loop_low_priority(void);

  //Struct con info de todos los sensores
  struct values
  {
    uint8_t _TPS;
    uint16_t _MAP;
    uint16_t TEMP;
    uint16_t IAT;
  } values{0, 0, 0, 0};

} // namespace sensors

#endif