#include "defines.h"
#include "user_defines.h"
#include <stdint.h>

#include "../include/map.hpp"
#include "../utils/basic_electronics.h"
#include "dtc_codes.h"

int32_t MAP::get_value(int32_t filt_input) {

  int32_t sensor_volt = convert_to_volt(filt_input) * 1.534;

  return (int32_t)MAP_CAL(sensor_volt) / 10;
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
