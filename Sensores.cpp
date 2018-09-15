// 
// Esta libreria se encarga de administrar los sensores, listo ?)
// 
#include "Sensores.h"
#include "pines.h"

Sensores::Sensores(byte num) {

}
void Sensores::Setup(void) {

}

float Sensores::Temp(float) {
	//esta funcion mide la temperatura y devuelve float en celsius
	//**solo para sensor LM35**
#if alpha == 0
	int value = analogRead(TEMP2);
	float millivolts = (value / 1023.0) * 5000;
	float celsius = millivolts / 10;
	return celsius;
#endif

#if alpha == 1
	int val = analogRead(TEMP2);
	return map(val, 0, 1023, 0, 120);
#endif
}

int Sensores::vmar(int) {
	//funcion para obtener la posicion de la mariposa de aceleracion
#if alpha == 1
	int val = analogRead(TPS1);
	return map(val, 0, 1023, 0, 99);
#endif
}