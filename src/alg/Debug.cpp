#include "Debug.h"

Debug::Debug(interfazSerial& ser, Memory & mem3){
}

void Debug::loop(int _RPM, int time, int _TEMP){
	//este void se encarga de la lectura de DTC's, y modificacion de tablas
	//ademas informa el estado del sistema a OpenEFI || Tuner
	if (ser.MSG()){

	}

	if (millis() - prev >= 500) {
		prev = millis();
		ser.Enviar(000, "RPM", _RPM);
		ser.Enviar(000, "INY", time);
		ser.Enviar(000, "TMP", _TEMP);
	}

}

//
//Debug::~Debug(){
//}