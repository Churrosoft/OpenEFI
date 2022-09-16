/*
    todo el codigo relacionado al control del sensor "IAT"
*/
#include "../include/iat.hpp"

int32_t IAT::last_value = 0;

int32_t IAT::get_value(int32_t filt_input) {
  IAT::last_value = (int32_t)IAT_CAL(filt_input);
  return last_value;
}

int32_t IAT::get_calibrate_value(int32_t filt_input) {
  return (int32_t)IAT_CAL(filt_input) * 100;
}

dtc_data IAT::dtc() {

  if (IAT::last_value > IAT_MAX) {
    return dtc_data DTC_IAT_SENSOR_HIGH;
  }
  if (IAT::last_value < IAT_MIN && IAT::last_value > IAT_OPEN) {
    return dtc_data DTC_IAT_SENSOR_LOW;
  } else if (IAT::last_value > IAT_OPEN) {
    return dtc_data DTC_IAT_SENSOR_OPEN;
  }
  return dtc_data EMPTY_DTC;
}