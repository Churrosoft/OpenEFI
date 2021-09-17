
#include "defines.h"
#include "user_defines.h"
#include "variables.h"
#include "../sensors/sensors.hpp"
#include "../cpwm/include/cpwm.hpp"

/** @brief Loop para funciones que utilizan el systick:
 */
void timer_schedule(void);