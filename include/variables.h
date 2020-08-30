/*-----( Variables Globales )-----*/
//estas variables las puede usar cualquieeer funcion
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#ifndef VARIABLES
#define VARIABLES

extern int _RPM, //las rpm :V
	_POS,		 //posicion del cigueñal (en dientes) (el contador de rpm la resetea a cada rato)
	_AE;		 //avance de encendido

extern bool
	SINC; //indica si se sincronizo el PMS

/*-----( Variables RPM )-----*/

extern uint64_t T_RPM_AC; //para saber tiempo actual
extern uint64_t T_RPM_A;  //para saber tiempo anterior

/*-----( Variables sincronizado )-----*/
extern bool sincB;
extern bool initsinc;
extern unsigned long T1;
extern unsigned long T2;
extern unsigned long Ta;
extern unsigned long Tb;
/*-----( Variables Timer Global )-----*/
struct UtilsTimer
{
	uint64_t micros;
	uint32_t millis;
	uint32_t temp; // NO USAR, se reinicia cada 1mS para millis
};
typedef struct UtilsTimer UtilsTimer;
extern UtilsTimer Time;

#endif