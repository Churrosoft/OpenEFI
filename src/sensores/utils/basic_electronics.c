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


uint16_t convert_to_resistance(uint16_t adcval){
	return (uint16_t)((R1 * ADC_MAX_VALUE) / adcval) - R1; //para resistencia pullUp dividir R1
}

uint16_t convert_to_volt(uint16_t in){
	return (uint16_t)(in * Vref / ADC_MAX_VALUE);
}

uint16_t thermistor_get_temperature(int16_t adcval){
	// http://en.wikipedia.org/wiki/Steinhart–Hart_equation
	// C standard equivalent:
	// float logR = log(resistance);
	// float kelvin = 1.0 / (A + B * logR + C * logR * logR * logR );
	// kelvin -= V*V/(K * R)*1000; //auto calibracion , no podia ser tan facil no?, numeros con 0.XX rompen la FPU y no se pueden restar
	//--------------------------------------------------------------------------------------------
	// qfp_fadd: suma | qfp_fdiv: division | qfp_fmul: multiplicacion | qfp_fln: logaritmo natural
	float R = convert_to_resistance(adcval);
	float logR  = qfp_fln(R);
	float V = convert_to_volt(adcval);
 	float _B = 0.6;
  	float kelvin = qfp_fdiv( 1.0 , qfp_fadd( qfp_fadd(A , qfp_fmul(B , logR) ) ,qfp_fmul( qfp_fmul( qfp_fmul(C , logR) , logR) , logR )));
	
	//kelvin -= qfp_fexp(qfp_fsub(qfp_fln(V*V * 1000), qfp_fln(K * R *1000))); //version mixed, con C standard y qfp para operaciones pesadas
	//kelvin = kelvin - (qfp_fexp(qfp_fln(qfp_fmul(qfp_fmul(V,V) , 1000)) - qfp_fln(qfp_fmul(qfp_fmul(K , R) ,1000))));
	return (uint16_t)(kelvin - 273.15 )*100;
	//float logR  = qfp_fln(R);
	//float _A = 2.5;
	//return (uint16_t)(_A - logR);
}

#endif