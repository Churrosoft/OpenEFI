// DTC.h

#ifndef _DTC_h
#define _DTC_h

#include "Sensores.h"
#include "Memory.h"

class DTC {
public:
	Memory & mem2;
	Sensores & s3;
	DTC( Memory & mem2 , Sensores & s3);
	void DTC_Loop();
};

#endif
