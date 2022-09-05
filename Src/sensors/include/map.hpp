#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "defines.h"
#include "user_defines.h"

#include "dtc_codes.h"

extern "C" {
#include "../utils/basic_electronics.h"
}

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
    int32_t get_value(int32_t);

    /** @brief devuelve valor mas exacto para calibracion inicial
     * dependiendo las opciones de compilacion se agrega:
     * -healt check
     * @return valores de -15 a 120
     */
    int32_t get_calibrate_value(int32_t);

    /** @brief revisa el estado del sensor para disparar un DTC en caso que sea necesario
     * @param in valor en Volt del sensor
     */
    NEW_DTC dtc(void);
    extern int32_t last_value;
} // namespace MAP

#endif