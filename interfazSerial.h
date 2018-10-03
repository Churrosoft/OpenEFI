#pragma once
#ifndef _INTERFAZSERIAL_h
#define _INTERFAZSERIAL_h
#include "Arduino.h"
class interfazSerial{

public:
	interfazSerial(byte d);
	void Enviar(byte code, String SubCode, String msg);
	byte Getcode();
	String GetSubCode();
	String GetMSG();
	void loop();
private:
	String msg;
	bool _msg = false;
	String temp;
};

#endif