// 
// Esta libreria se encarga de administrar los sensores, listo ?)
// 
#include "Sensores.h"
#include "pines.h"
#include "TMAP.h" //modificar para usar ports
#include "CD4051.h"
CD4051 MUX(0);
TMAP myMAP(MUX);

Sensores::Sensores(byte num) {
}

int Sensores::Temp() {
	//esta funcion mide la temperatura y devuelve float en celsius
	//**solo para sensor LM35**
#if alpha == 0
	//int value = analogRead(TEMP2);
	float mV = (MUX.GetVal(1) / 1023.0) * 5000;
	return mV * 10;
#endif

#if alpha == 1
	int val = MUX.GetVal(1);
	return map(val, 0, 1023, -15, 125); //igual habria que revisar esta conversion teniendo en cuenta un sensor ntc
#endif
}

int Sensores::Temp(bool op){
	return (MUX.GetVal(1) / 1023.0) * 50000;
}

int Sensores::tps() {
	//funcion para obtener la posicion de la mariposa de aceleracion
#if alpha == 1
	//return map(analogRead(TPS1) , 0, 1023, 0, 100);
	return map(MUX.GetVal(0), 0, 1023, 0, 100);
#endif
	return 0;
}

int Sensores::tps(bool op){
	//esto devuelve mV del sensor nomas, lo uso para el DTC
	return (MUX.GetVal(0)  * (5 / 1023)) * 100;
}

int Sensores::lamb(byte n){
	//luego hago que mande valor lambda en vez de volt
#if OpenEFImode == 0
	if (n == 0) {
		return (MUX.GetVal(4)  * (5/ 1023)) * 100;
		//return (analogRead(LMBA)  * (3.3 / 1023)) * 100;
	}
	if (n == 1) {
		//return (analogRead(LMBB)  * (3.3 / 1023)) * 100;
		return (MUX.GetVal(5)  * (5 / 1023)) * 100;
	}
#endif
	return 0;
}

int Sensores::lamb(byte n, bool op){
	if (n == 0) {
		return (MUX.GetVal(4)  * (5 / 1023)) * 100;
		//return (analogRead(LMBA)  * (3.3 / 1023)) * 100;
	}
	if (n == 1) {
		//return (analogRead(LMBB)  * (3.3 / 1023)) * 100;
		return (MUX.GetVal(5)  * (5 / 1023)) * 100;
	}
}

int Sensores::pres() {	
	return myMAP.MasaAire() * 100;
}

int Sensores::pres(bool op){
	return (MUX.GetVal(6)  * (5 / 1023)) * 100;
}

int Sensores::airTemp(bool op){
	return (MUX.GetVal(2)  * (5 / 1023)) * 100;
}
