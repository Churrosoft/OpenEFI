//
//Debug.h se encarga de DTC's y modificacion de tablas
//
#pragma once
#include "interfazSerial.h"
#include "Memory.h"
class Debug {
public:
	interfazSerial& ser;
	Memory & mem3;
	Debug(interfazSerial& ser, Memory & mem3);
	void loop(int _RPM, int time, int _TEMP);
	//virtual ~Debug();
private:
	unsigned long prev = 0;
};