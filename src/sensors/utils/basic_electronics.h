#ifndef BASIC_ELECTRONIC_MATH
#define BASIC_ELECTRONIC_MATH
#include <stdint.h>

/** @brief Convierte valor del ADC a volt:
 *  @param in : valor del adc a convertir
 *  @return mV convertidos
 */
uint16_t convert_to_volt(uint16_t);

/** @brief Convierte valor del ADC a resistencia:
 *  @param in : valor del adc a convertir
 *  @return resistencia en Ohm
 */
uint16_t convert_to_resistance(uint16_t);

uint16_t thermistor_get_temperature(int16_t);

#endif