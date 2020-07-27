#include <stdint.h>
#include "defines.h"
#include "../utils/basic_electronics.h"
#include "../../qfplib/qfplib-m3.h"

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
    uint16_t get_value(uint16_t);

    /** @brief revisa el estado del sensor para disparar un DTC en caso que sea necesario
     * @param in valor en Volt del sensor
     */
    uint8_t dtc(uint16_t);
} // namespace TEMP

#endif