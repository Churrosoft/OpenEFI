#ifndef BASIC_ELECTRONIC_MATH
#define BASIC_ELECTRONIC_MATH

#include <stdint.h>
// custom:
#include "./ema_low_pass.c"
#include "../../defines.h"
#include "../../../qfplib/qfplib-m3.h"

uint16_t convert_to_resistance(uint16_t adcval){
	return (uint16_t)((R1 * ADC_MAX_VALUE) / adcval) - R1; //para resistencia pullUp dividir R1
}

uint16_t convert_to_volt(uint16_t in){
	return (uint16_t)(in * Vref / ADC_MAX_VALUE);
}

uint16_t thermistor_get_temperature(int32_t resistance){
	// http://en.wikipedia.org/wiki/Steinhart–Hart_equation
	// C standard equivalent:
	//float T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));

	float logR2 = qfp_fln(resistance);
	float T = qfp_fdiv(1.0, (c1 + qfp_fmul(c2, logR2) + qfp_fmul(c3, qfp_fmul(qfp_fmul(logR2, logR2), logR2))));
	// convertimos a C° y multiplicamos por 100
	T = (T -273.15) * 100;
	return (uint16_t)T;
}

#endif