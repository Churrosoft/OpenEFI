#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include "sensors/sensors.hpp"
#include "variables.h"

/** @brief Loop para funciones que utilizan el systick:
 */
void timer_schedule(void);