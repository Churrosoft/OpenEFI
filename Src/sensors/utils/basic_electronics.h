#ifndef BASIC_ELECTRONIC_MATH
#define BASIC_ELECTRONIC_MATH
#include <stdint.h>

/** @brief Convierte valor del ADC a volt:
 *  @param in : valor del adc a convertir
 *  @return mV convertidos
 */
int32_t convert_to_volt(int32_t);

/** @brief Convierte valor del ADC a resistencia:
 *  @param in : valor del adc a convertir
 *  @return resistencia en Ohm
 */
int32_t convert_to_resistance(int32_t);

int32_t thermistor_get_temperature(int32_t);

#endif