#ifndef SENSORS_CPP
#define SENSORS_CPP

#include "./input_handler.hpp"
#include "defines.h"
#include "user_defines.h"

// sensores:
#include "./include/batt.hpp"
#include "./include/iat.hpp"
#include "./include/map.hpp"
#include "./include/temp.hpp"
#include "./include/tps.hpp"

#include <stdint.h>

namespace sensors {
/**
 * @brief Setup inicial de todos los sensores
 */
void setup(void);

/**
 * @brief Loop principal de los sensores, corre cada 50-75mS
 */
void loop(void);

/** @brief Loop secundario, corre cada 250mS */
void loop_low_priority(void);

// Struct con info de todos los sensores
typedef struct {
  int32_t _TPS;
  int32_t _MAP;
  int32_t TEMP;
  int32_t IAT;
  int32_t BATT;
  int32_t LMB;
} SensorValues;
// typedef struct SensorValues SensorValues;

extern SensorValues values;

} // namespace sensors

#endif