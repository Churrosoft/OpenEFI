//
// Libreria para control de avance
//
#include "AVC.h"

AVC::AVC(byte dientes){
	dnt = dientes;
}

byte AVC::GetAVC(int rpm, byte temp) {
	if (rpm < 600) {
		return AVC::dientes(3);

	}
	return 0;
}

//convierte grados en dientes del sensor hall
byte AVC::dientes(float grados) {
	//codigo viejo
	//float grad = 360 / dnt; //dividimos 360 grados por la cantidad de dientes
	//						//dividimos grados por grad, luego multiplicamos por 100 para transformar el float en int
	//int x2 = (grados / grad) * 100;
	////dividimos por 100, al hacer esto se eliminan los decimales, en prox ver redondear
	//int dnt2 = x2 / 100;
	//return dnt2;
	return (grados * (360 / dnt));
}