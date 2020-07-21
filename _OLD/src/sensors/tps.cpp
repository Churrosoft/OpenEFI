#ifndef TPS_CPP
#define TPS_CPP
#include <stdint.h>
#include "defines.h"
#include "./utils/basic_electronics.c"
/*
    todo el codigo relacionado al control del sensor "TPS"
*/
namespace TPS
{
    /** @brief devuelve valor de 0 a 100 representando la apertura de la mariposa 
     * (tanto para mariposa motorizada como a cable)
     * dependiendo las opciones de compilacion se agrega:
     * -healt check
     * -doble medicion con otra entrada analogica
     * @return escala de 0 a 100
     */
    uint8_t get_value(uint16_t);
    uint8_t get_value(uint16_t, uint16_t);

    /** @brief devuelve valor mas exacto para calibracion inicial de la mariposa
     * incluye overload en caso de sensor de TPS doble
     * @return escala de 0 a 100 con 3 digitos de precision
     */
    uint32_t get_calibrate_value(uint16_t);
    uint32_t get_calibrate_value(uint16_t, uint16_t);

    /** @brief revisa el estado del sensor para disparar un DTC en caso que sea necesario
     */
    uint8_t dtc(uint16_t);
    uint8_t dtc(uint16_t, uint16_t);
} // namespace TPS

uint8_t TPS::get_value(uint16_t filt_input)
{
    uint16_t _t = convert_to_volt(filt_input) * 1334;
    if (_t > TPS_MAX_A || _t < TPS_MIN_A)
        return 0;
    return (uint8_t)TPS_CALC_A(_t);
}

uint32_t TPS::get_calibrate_value(uint16_t filt_input)
{
    uint16_t _t = convert_to_volt(filt_input) * 1334;
    if (_t > TPS_MAX_A || _t < TPS_MIN_A)
        return 0;
    return (uint32_t)TPS_CALC_A(_t) * 1000;
}

uint8_t TPS::dtc(uint16_t in1)
{
    if (in1 > TPS_MAX_A)
        return 1;
    if (in1 < TPS_MIN_A)
        return -1;
    return 0;
}

uint8_t TPS::dtc(uint16_t in1, uint16_t in2)
{
    //TODO: laburar x1
    return 0;
}
uint8_t TPS::get_value(uint16_t filt_input, uint16_t filt_input2)
{
    //TODO: laburar x2
    return 0;
}

uint32_t TPS::get_calibrate_value(uint16_t filt_input, uint16_t filt_input2)
{
    //TODO: laburar x3
    return 0;
}

#endif