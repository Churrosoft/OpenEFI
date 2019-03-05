//el uso de esta libreria esta definido en el archivo .h
#include "interfazSerial.h"

interfazSerial::interfazSerial(byte d){
	if(d == 2){
		Serial.begin(11520);
	}
	Serial.begin(11520);
}

void interfazSerial::Enviar(byte code, String SubCode, String msg){
	//este void caza todo, crea el mensaje y lo manda
	temp = "";
	temp += code;
	temp += SubCode;
	temp += msg;
	Serial.println(temp);
}

void interfazSerial::Enviar(byte code, String SubCode, int msg) {
	//este void caza todo, crea el mensaje y lo manda
	temp = "";
	temp += code;
	temp += SubCode;
	temp += msg;
	Serial.println(temp);
}


int interfazSerial::Getcode(){ 
	if (_msg) {
		temp = msg;
		temp.remove(3, 20);
		//devuelvo codigo del ultimo mensaje recibido
		return temp.toInt();
	}
	return -1;
}

String interfazSerial::GetSubCode(){
	//devuelvo subcodigo del ultimo mensaje recibido
	if (_msg) {
		temp = msg;
		temp.remove(0, 3);
		temp.remove(4, 20);
		return temp;
	}
	return "-1";
}

String interfazSerial::GetMSG(){
	//devuelvo la info del ultimo mensaje
	if (_msg) {
		temp = msg;
		temp.remove(0, 5);
		return temp;
	}
	return "-1";
}

int interfazSerial::GetMSG_int() {
	//devuelvo la info del ultimo mensaje
	if (_msg) {
		temp = msg;
		temp.remove(0, 5);
		return temp.toInt();
	}
	return -1;
}

void interfazSerial::loop(){
	//loop para actualizar mensajes en cola
	while (Serial.available()) {
		char _T = (char)Serial.read();
		msg += _T;
		if (_T == '\n') {
			_msg = true;
		}
	}
}

bool interfazSerial::MSG(){
	return _msg;
}


void interfazSerial::MSG(bool op){
	_msg = op;
}

bool interfazSerial::FXMD(){
	if (msg) {
		if (interfazSerial::Getcode() == 004) {
			if (interfazSerial::GetSubCode().startsWith("FXM_E")) {
				return true;
			}
			if (interfazSerial::GetSubCode().startsWith("FXM_D")) {
				return false;
			}
		}
	}
	return true;
}

int interfazSerial::FXMD(bool mode, int val) {
	if (_msg) {
		String temp2 = "";
		if (interfazSerial::Getcode() == 004) {
			if (interfazSerial::GetSubCode().startsWith("FXM_A")) {
				if (!mode) {
					//solo convierto el valor y retorno cuando corresponda
					_msg = false;
					return interfazSerial::GetMSG_int();
				}
			}
			if (interfazSerial::GetSubCode().startsWith("FXM_I")) {
				if (mode) {
					//solo convierto el valor y retorno cuando corresponda
					_msg = false;
					return interfazSerial::GetMSG_int();
				}
			}
		}
	}
	return val;
}