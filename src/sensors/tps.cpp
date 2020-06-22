#ifndef TPS_CPP
#define TPS_CPP
#include <stdint.h>
#include <compare>
/*
    todo el codigo relacionado al control del sensor "TPS"
*/
namespace TPS
{
    /** @brief devuelve valor de 0 a 255 representando la apertura de la mariposa 
     * (tanto para mariposa motorizada como a cable)
     * dependiendo las opciones de compilacion se agrega:
     * -healt check
     * -doble medicion con otra entrada analogica
     * @return escala de 0 a 255
     */
    uint8_t get_value(uint16_t);
    uint8_t get_value(uint16_t, uint16_t);
    /** @brief devuelve valor mas exacto para calibracion inicial de la mariposa
     * incluye overload en caso de sensor de TPS doble
     */
    uint16_t get_calibrate_value(uint16_t);
    uint16_t get_calibrate_value(uint16_t, uint16_t);
} // namespace TPS

uint8_t TPS::get_value(uint16_t filt_input)
{
    uint16_t test1 = 4, test2 = 54;
    auto result = test1 <=> test2;
    return 4;
}

uint8_t TPS::get_value(uint16_t filt_input, uint16_t filt_input2)
{
    return 2;
}

#endif