#include "../include/temp.hpp"
#include <math.h>

uint16_t TEMP::get_value(uint16_t filt_input) {
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

  return (uint16_t)(kelvin - 273.15) * 100;
}

uint8_t TEMP::dtc(uint16_t in) {
  if (in > TEMP_MAX)
    return 1;
  if (in < TEMP_MIN)
    return -1;
  return 0;
}
