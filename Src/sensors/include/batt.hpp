#ifndef BATT_H
#define BATT_H
#include "defines.h"
#include "user_defines.h"
#include <stdint.h>

#include "dtc_codes.h"

extern "C" {
#include "../utils/basic_electronics.h"
}

/*
    todo el codigo relacionado al control del sensor "BATT"
*/
namespace BATT {
/** @brief devuelve valor en mV de la bateria
 */
int32_t get_value(int32_t);
int32_t get_value(int32_t, int32_t);

/** @brief revisa el estado del sensor para disparar un DTC en caso que sea
 * necesario
 */
dtc_data dtc(void);
extern int32_t last_value;
} // namespace BATT

#endif