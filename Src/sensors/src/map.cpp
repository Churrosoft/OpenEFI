#include "defines.h"
#include "user_defines.h"
#include <stdint.h>

#include "../include/map.hpp"
#include "../utils/basic_electronics.h"
#include "dtc_codes.h"

int32_t MAP::get_value(int32_t filt_input) {
  return filt_input;
  int32_t volatile debug_a = convert_to_volt(filt_input);
  debug_a = debug_a;
  return (int32_t)MAP_CAL(convert_to_volt(filt_input));
}

int32_t MAP::get_calibrate_value(int32_t filt_input) {
  return (int32_t)MAP_CAL(filt_input) * 100;
}

uint8_t *MAP::dtc() {
  if (MAP::last_value > MAP_MAX)
    return NEW_DTC DTC_MAP_SENSOR_HIGH;
  if (MAP::last_value < MAP_MIN)
    return NEW_DTC DTC_MAP_SENSOR_LOW;
  return 0;
}
