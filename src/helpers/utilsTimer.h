#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include "variables.h"

#ifndef UTILSTIMERw
#define UTILSTIMERw
/*
 *Inicia el systick para contar los uS | mS desde que inicio todo
 */
void utils_timer_setup(void);

/** @brief devuelve mS desde que se prendio el mcu
 */
uint32_t millis(void);

#endif