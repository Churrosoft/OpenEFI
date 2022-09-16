#include "../include/batt.hpp"
#include "dtc_codes.h"

int32_t BATT::last_value = 0;

int32_t BATT::get_value(int32_t filt_input) {
  int32_t _t = convert_to_volt(filt_input);

  // V = _t * (r1 + r2) / r2
  last_value = _t * (100000 + 20000) / 20000;

  return (int32_t)last_value;
}
