// Sensores.h
#ifndef _SENSORES_h
#define _SENSORES_h
#include "arduino.h"

class Sensores{
	public:
		Sensores(byte num);
		int Temp();
		int Temp(bool op);
		int tps();
		int tps(bool op);
		int lamb(byte n); //sensor lambda, numero indica cual necesito medir
		int lamb(byte n, bool op);
		int pres();
		int pres(bool op);
		int airTemp(bool op);
	private:
};

#endif