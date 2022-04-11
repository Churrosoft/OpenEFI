#ifndef TPS_H
#define TPS_H
#include <stdint.h>
#include "defines.h"
#include "user_defines.h"

#include "dtc_codes.h"

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
    int32_t get_value(int32_t);
    int32_t get_value(int32_t, int32_t);

    /** @brief devuelve valor mas exacto para calibracion inicial de la mariposa
     * incluye overload en caso de sensor de TPS doble
     * @return escala de 0 a 100 con 3 digitos de precision
     */
    int32_t get_calibrate_value(int32_t);
    int32_t get_calibrate_value(int32_t, int32_t);

    /** @brief revisa el estado del sensor para disparar un DTC en caso que sea necesario
     */
    NEW_DTC dtc(void);
    uint8_t dtc(int32_t, int32_t);
    extern int32_t last_value;
} // namespace TPS

#endif