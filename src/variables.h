/*-----( Variables Globales )-----*/
//estas variables las puede usar cualquieeer funcion
#include <stdlib.h>

#ifndef VARIABLES
#define VARIABLES

int _RPM = -4646, //las rpm :V
	_TEMP = 85,	  //temperatura
	_POS = 0,	  //posicion del cigueñal (en dientes) (el contador de rpm la resetea a cada rato)
	_AE = 0,	  //avance de encendido
	_V00 = -4646; //voltaje de bateria, multiplicado por 10 para evitar double
bool
	SINC = false;   //indica si se sincronizo el PMS

/*-----( Variables RPM )-----*/

long T_RPM_AC  = 0;   		  //para saber tiempo actual
long T_RPM_A   = 0;  		  //para saber tiempo anterior

/*-----( Variables sincronizado )-----*/
bool sincB = false;
bool initsinc = false;
unsigned long T1;
unsigned long T2;
unsigned long Ta = 0;
unsigned long Tb = 0;
/*-----( Variables Timer Global )-----*/
struct UtilsTimer{
	uint64_t micros;
	uint32_t millis;
	uint32_t temp; // NO USAR, se reinicia cada 1mS para millis
}Time = {0,0,0};

#endif