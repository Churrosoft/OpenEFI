#include "Debug.h"

Debug::Debug(interfazSerial& ser, Memory & mem3){
}

void Debug::loop(int _RPM){
	//este void se encarga de la lectura de DTC's, y modificacion de tablas
	//ademas informa el estado del sistema a OpenEFI || Tuner
	if (ser.MSG()){

	}

	if (millis() - prev >= 500) {
		// save the last time you blinked the LED
		prev = millis();
		ser.Enviar(000, "RPM", _RPM);
	}

}

//
//Debug::~Debug(){
//}