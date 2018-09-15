// Sensores.h
#ifndef _SENSORES_h
#define _SENSORES_h
#include "arduino.h"

class Sensores{
	public:
		Sensores(byte num);
		void  Setup(void);
		float Temp(float);
		int vmar(int);
	private:
};

#endif