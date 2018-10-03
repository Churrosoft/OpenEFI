#pragma once
#ifndef _AVC_h
#define _AVC_h

#include "Arduino.h"
class AVC{

public:
	AVC(byte dientes);
	byte GetAVC(int rpm, byte temp); //recupero el tiempo
private:
	byte dnt; //dientes del cigueñal
	byte dientes(float grados);
};

#endif