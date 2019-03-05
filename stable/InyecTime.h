// InyecTime.h

#ifndef _INYECTIME_h
#define _INYECTIME_h
#include "Sensores.h"
#include "Memory.h"

class InyecTime {
public:
	Sensores& s1;
	Memory& mI;
	InyecTime(Sensores& s1, Memory& mI);
	unsigned long Tarr(); //Genero tiempo para arranque
	unsigned long TLamb(unsigned long Tbase, byte temp);//correcion por lambda
	unsigned long TTable(int rpm); //valor de la tabla dependiendo rpm
	unsigned long Temp(int rpm); //correcion de tiempo por temperatura
	unsigned long Aphplus(int rpm); //Aplha-N
	unsigned long BPW();
private:
	int CLT;
	float VtoLamb(float v);
};

#endif