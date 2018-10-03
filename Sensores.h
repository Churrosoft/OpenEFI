// Sensores.h
#ifndef _SENSORES_h
#define _SENSORES_h
#include "arduino.h"

class Sensores{
	public:
		Sensores(byte num);
		int Temp();
		int tps();
		int lamb(byte n); //sensor lambda, numero indica cual necesito medir
		int pres();
	private:
};

#endif