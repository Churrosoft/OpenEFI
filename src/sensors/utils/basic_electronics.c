
// custom:
#include "./ema.h"
#include "defines.h"
#include "../../../qfplib/qfplib-m3.h"

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
