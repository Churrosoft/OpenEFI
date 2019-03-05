#pragma once
#ifndef _AVC_h
#define _AVC_h

#include "Arduino.h"
#include "Sensores.h"
#include "Memory.h"
class AVC{
public:
	Sensores& s2;
	Memory& ms;
	AVC(byte dientes, Sensores& s2, Memory& ms);
	byte GetAVC(int rpm); 
	byte GetTime();//recupero el tiempo
private:
	byte dnt; //dientes del cigueñal
	byte dientes(float grados);
};

#endif