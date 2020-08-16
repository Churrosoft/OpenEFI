// STDLIB
#include <stdbool.h>
#include <stdint.h>

#include "defines.h"
#include "fast_math.h"
#include "utilsTimer.h"
#include "variables.h"

#ifndef CPWM_UTILS
#define CPWM_UTILS

/** @brief convierte grados en dientes del volante de inercia
 * @param grad grados a convertir
 * @return dientes equivalentes al angulo
 */
uint_fast16_t grad_to_dnt(float);

/** @brief convierte dientes en grados del volante de inercia
 * @param _dnt dientes a convertir
 * @return grados equivalentes al angulo
 */
uint16_t dnt_to_grad(uint16_t);

/** @brief calcula tiempos entre los ultimos dos dientes para poder sincronizar
 * el cigueñal
 * @return true en caso que el diente anterior sea el PMS
 */
bool sinc();

/** @brief void boludo para calcular las rpm, recuperado de las profundidades
 * del git
 * */
void RPM();

#endif