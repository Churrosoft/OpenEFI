#pragma once
#ifndef _MEMORY_h
#define _MEMORY_h
#include "Arduino.h"
class Memory{
public:
	Memory(int p);
	void Actualizar(int val, int x, int y);
	int GetVal(byte tabla, byte x, byte y);
	void Leer(int **tabla, int tipo);
	int *GetDTC();
	void SetDTC(int code);
private:
	int i = 0;
	int i2 = 0;
	int pos = 0;
};
#endif
