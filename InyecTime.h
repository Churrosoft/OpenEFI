// InyecTime.h

#ifndef _INYECTIME_h
#define _INYECTIME_h
#include "arduino.h"
#include "Sensores.h"
class InyecTime {
	InyecTime();
	unsigned long Tarr(); //Genero tiempo para arranque
	unsigned long TLamb(unsigned long Tbase, byte temp); //correcion por lambda
	unsigned long Tiny(byte vmar, byte rpm); //tiempo de inyeccion a seca (?)
	unsigned long Temp(byte vmar, byte rpm, byte temp); //correcion de tiempo por temperatura
	unsigned long Aphplus(byte vmar, byte rpm, byte temp, byte map, int VE[10][18]); //Aplha-N
	unsigned long BPW();
};

#endif