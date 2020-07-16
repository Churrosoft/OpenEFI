#ifndef BASIC_ELECTRONIC_MATH
#define BASIC_ELECTRONIC_MATH

#include <stdint.h>
// custom:
#include "./ema_low_pass.c"
#include "defines.h"
#include "../../../qfplib/qfplib-m3.h"

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

uint16_t convert_to_resistance(uint16_t adcval)
{
#if defined(INPUT_PULL_UP)
	return (uint16_t)((R1 * ADC_MAX_VALUE) / adcval) / R1; //para resistencia pullUp dividir R1
#else
	return (uint16_t)((R1 * ADC_MAX_VALUE) / adcval) - R1; //para resistencia pullDown multiplicar R1
#endif
}

uint16_t convert_to_volt(uint16_t in)
{
	return (uint16_t)(in * Vref / ADC_MAX_VALUE);
}

#endif