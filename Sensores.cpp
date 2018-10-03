// 
// Esta libreria se encarga de administrar los sensores, listo ?)
// 
#include "Sensores.h"
#include "pines.h"
//#include "TMAP.h" modificar para usar ports
#include "CD4051.h"
//TMAP myMAP(1, 2);
CD4051 MUX(0);
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
	return map(val, 0, 1023, 0, 120);
#endif
}

int Sensores::tps() {
	//funcion para obtener la posicion de la mariposa de aceleracion
#if alpha == 1
	//return map(analogRead(TPS1) , 0, 1023, 0, 100);
	return map(MUX.GetVal(0), 0, 1023, 0, 100);
#endif
	return 0;
}

int Sensores::lamb(byte n){
	//luego hago que mande valor lambda en vez de volt
#if OpenEFImode == 0
	if (n == 0) {
		return (MUX.GetVal(4)  * (3.3 / 1023)) * 100;
		//return (analogRead(LMBA)  * (3.3 / 1023)) * 100;
	}
	if (n == 1) {
		//return (analogRead(LMBB)  * (3.3 / 1023)) * 100;
		return (MUX.GetVal(5)  * (3.3 / 1023)) * 100;
	}
#endif
	return 0;
}

int Sensores::pres() {	
	//return myMAP.MasaAire() * 100;
}