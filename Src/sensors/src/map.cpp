#include "../include/map.hpp"

int32_t MAP::get_value(int32_t filt_input) {

  int32_t sensor_volt = convert_to_volt(filt_input) * 1.534;

  return (int32_t)MAP_CAL(sensor_volt) / 10;
}

int32_t MAP::get_calibrate_value(int32_t filt_input) {
  return (int32_t)MAP_CAL(filt_input) * 100;
}

NEW_DTC MAP::dtc() {
  if (MAP::last_value > MAP_MAX)
    return NEW_DTC DTC_MAP_SENSOR_HIGH;
  if (MAP::last_value < MAP_MIN)
    return NEW_DTC DTC_MAP_SENSOR_LOW;
  return NEW_DTC EMPTY_DTC;
}
