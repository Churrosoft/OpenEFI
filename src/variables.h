/*-----( Variables Globales )-----*/
//estas variables las puede usar cualquieeer funcion

#ifndef VARIABLES
#define VARIABLES

int _RPM       = 0, 			//las rpm :V
	_TEMP      = 0, 			//temperatura
	_POS       = 0, 			//posicion del cigueñal (en dientes) (el contador de rpm la resetea a cada rato)
	_AE        = 0; 			//avance de encendido
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
#endif