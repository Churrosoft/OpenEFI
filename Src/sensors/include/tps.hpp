#ifndef TPS_H
#define TPS_H
#include <stdint.h>
#include "defines.h"
#include "../utils/basic_electronics.h"
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

#endif