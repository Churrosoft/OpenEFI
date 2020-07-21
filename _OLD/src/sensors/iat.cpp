#ifndef IAT_CPP
#define IAT_CPP
#include <stdint.h>
#include "defines.h"
#include "./utils/basic_electronics.c"
/*
    todo el codigo relacionado al control del sensor "IAT"
*/
namespace IAT
{
    /** @brief devuelve valor en kPa del sensor
     * dependiendo las opciones de compilacion se agrega:
     * -healt check
     * @return valores de 0 a 260
     */
    uint16_t get_value(uint16_t);

    /** @brief devuelve valor mas exacto para calibracion inicial
     * dependiendo las opciones de compilacion se agrega:
     * -healt check
     * @return valores de -15 a 120
     */
    uint32_t get_calibrate_value(uint16_t);

    /** @brief revisa el estado del sensor para disparar un DTC en caso que sea necesario
     * @param in valor en Volt del sensor
     */
    uint8_t dtc(uint16_t);
} // namespace IAT

uint16_t IAT::get_value(uint16_t filt_input)
{
    return (uint16_t)IAT_CAL(filt_input);
}

uint32_t IAT::get_calibrate_value(uint16_t filt_input)
{
    return (uint32_t)IAT_CAL(filt_input) * 100;
}

uint8_t IAT::dtc(uint16_t in)
{
    if (in > IAT_MAX)
        return 1;
    if (in < IAT_MIN)
        return -1;
    return 0;
}

#endif