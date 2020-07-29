#include "variables.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include "../sensors/sensors.hpp"

#ifndef UTILSTIMERw
#define UTILSTIMERw
/* 
 *Inicia el systick para contar los uS | mS desde que inicio todo
*/
void utils_timer_setup(void);

/** @brief Loop para funciones que utilizan el systick:
 */
void utils_timer_loop(void);

#endif