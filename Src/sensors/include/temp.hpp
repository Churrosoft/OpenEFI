#include <stdint.h>
#include <math.h>
#include "defines.h"
#include "user_defines.h"

#include "dtc_codes.h"

#include "../utils/basic_electronics.h"

#ifndef TEMP_CPP
#define TEMP_CPP

/*
    todo el codigo relacionado al control del sensor "TPS"
*/
namespace TEMP
{
    /** @brief devuelve valor en grados del sensor
     * dependiendo las opciones de compilacion se agrega:
     * -healt check
     * @return valores de -15 a 120
     */
    int32_t get_value(int32_t);

    /** @brief revisa el estado del sensor para disparar un DTC en caso que sea necesario
     * @param in valor en Volt del sensor
     */
   NEW_DTC dtc(void);

    extern int32_t last_value;
} // namespace TEMP

#endif