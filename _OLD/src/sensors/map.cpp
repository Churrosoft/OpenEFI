#ifndef MAP_CPP
#define MAP_CPP
#include <stdint.h>
#include "defines.h"
#include "./utils/basic_electronics.c"
/*
    todo el codigo relacionado al control del sensor "MAP"
*/
namespace MAP
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
} // namespace MAP

uint16_t MAP::get_value(uint16_t filt_input)
{
    return (uint16_t)MAP_CAL(filt_input);
}

uint32_t MAP::get_calibrate_value(uint16_t filt_input)
{
    return (uint32_t)MAP_CAL(filt_input) * 100;
}

uint8_t MAP::dtc(uint16_t in)
{
    if (in > MAP_MAX)
        return 1;
    if (in < MAP_MIN)
        return -1;
    return 0;
}

#endif