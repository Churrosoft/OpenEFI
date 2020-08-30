#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include "defines.h"
#include "sensors/sensors.hpp"
#include "variables.h"

extern "C" {
#include "c_pwm_utils.h"
}
/** @brief Loop para funciones que utilizan el systick:
 */
void timer_schedule(void);