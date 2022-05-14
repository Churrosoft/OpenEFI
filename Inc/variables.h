/*-----( Variables Globales )-----*/
//	estas variables las puede usar cualquieeer funcion
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef VARIABLES
#define VARIABLES

extern uint32_t _RPM, //	las rpm :V
    _POS,    //	posicion del cigueñal (en dientes) (el contador de rpm la resetea a cada rato)
    _POS_AE, //	posicion arbol de levas
    _DEG;
extern int32_t _AE; //	avance de encendido
extern uint32_t _rpm_time;


extern bool MOTOR_ENABLE, //	cache de "can_turn_on"
    SINC;                 //	indica si se sincronizo el PMS

/*-----( Variables RPM )-----*/

extern uint64_t T_RPM_AC; //	para saber tiempo actual
extern uint64_t T_RPM_A;  //	para saber tiempo anterior

/*-----( Variables sincronizado )-----*/
extern bool sincB;
extern bool initsinc;
extern unsigned long T1;
extern unsigned long T2;
extern unsigned long Ta;
extern unsigned long Tb;

#endif