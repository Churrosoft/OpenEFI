#ifndef BASIC_ELECTRONIC_MATH
#define BASIC_ELECTRONIC_MATH

#include <stdint.h>
// custom:
#include "./ema_low_pass.c"
#include "../../defines.h"
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

uint16_t thermistor_get_temperature(int16_t adcval)
{
	// http://en.wikipedia.org/wiki/Steinhart–Hart_equation
	// C standard equivalent:
	// float logR = log(resistance);
	// float kelvin = 1.0 / (A + B * logR + C * logR * logR * logR );
	// kelvin -= V*V/(K * R)*1000; // auto calibracion (a implementar)
	//--------------------------------------------------------------------------------------------
	// qfp_fadd: suma | qfp_fdiv: division | qfp_fmul: multiplicacion | qfp_fln: logaritmo natural
	float R = convert_to_resistance(adcval);
	float logR = qfp_fln(R);
	float kelvin = qfp_fdiv(1.0, qfp_fadd(qfp_fadd(A, qfp_fmul(B, logR)), qfp_fmul(qfp_fmul(qfp_fmul(C, logR), logR), logR)));

#ifdef NTC_AUTO_CALIB
	float V = convert_to_volt(adcval);
	kelvin = kelvin - (qfp_fexp(qfp_fln(qfp_fmul(qfp_fmul(V, V), 1000)) - qfp_fln(qfp_fmul(qfp_fmul(K, R), 1000))));
#endif

	return (uint16_t)(kelvin - 273.15) * 100;
}

#endif