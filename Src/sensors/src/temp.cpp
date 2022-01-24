#include "../include/temp.hpp"
#include "dtc_codes.h"
#include <math.h>

const uint32_t A = 1.12492089e-3;
const uint32_t B = 2.372075385e-4;
const uint32_t C = 6.954079529e-8;

int32_t TEMP::get_value(int32_t filt_input)
{
  // http://en.wikipedia.org/wiki/Steinhart–Hart_equation
  // C standard equivalent:
  // float logR = log(resistance);
  // float kelvin = 1.0 / (A + B * logR + C * logR * logR * logR );
  // kelvin -= V*V/(K * R)*1000; // auto calibracion (a implementar)
  //--------------------------------------------------------------------------------------------
  // qfp_fadd: suma | qfp_fdiv: division | qfp_fmul: multiplicacion | qfp_fln:
  // logaritmo natural
  float R = convert_to_resistance(filt_input);
  float logR = log(R);
  float kelvin = 1.0 / (A + B * logR + C * logR * logR * logR);

#ifdef NTC_AUTO_CALIB // WIP
  float V = convert_to_volt(adcval);
  kelvin -= V * V / (K * R) * 1000; // auto calibracion (a implementar)
#endif
  last_value = (int32_t)(kelvin - 273.15) * 100;
  return last_value;
}

uint8_t *TEMP::dtc()
{
  if (last_value > TEMP_MAX)
    return NEW_DTC DTC_ECT_SENSOR_HIGH;
  if (last_value < TEMP_MIN && last_value > TEMP_OPEN)
    return NEW_DTC DTC_ECT_SENSOR_LOW;
  else
    return NEW_DTC DTC_ECT_OUT_OF_RANGE;
  return 0;
}
