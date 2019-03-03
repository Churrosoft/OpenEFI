// CD4051.h libreria para manejar MUX analogico en la entrada A0

#ifndef _CD4051_h
#define _CD4051_h
#include "Arduino.h"
class CD4051{
public:
	CD4051(byte op);
	int GetVal(byte in); //obtengo el valor, in es el numero de entrada que quiero leer
private:
};
#endif
