/**
 * usa este archivo para aplicar una de las plantillas de defines, y/o modificarla
 *  
 */
#include <stdint.h>

#ifndef USER_DEFINES_H
#define USER_DEFINES_H

/** 
 * Descomenta (//) una de las lineas de import, dependiendo tu motor, consulta docs.openefi.tech para mas info
*/

/* Gol/ Senda/ Ford Escort Motor Renault CHT/AE 1600 */
#include "templates/CHT_1_6_.h"

// Ejemplo de modificacion:
#undef AVCI
#define AVCI 24
// {{USER_DEFINES_END}}
#endif