//
// Libreria para control de avance
//
#include "AVC.h"

AVC::AVC(byte dientes, Sensores& s2, Memory& ms){
	dnt = dientes;
}

byte AVC::GetTime(){
	//UNDONE: tiempo de encendido hace falta hacer que varie ?)
	return byte(200);
}

byte AVC::GetAVC(int rpm) {
	if (rpm < 600) 
		return AVC::dientes(-2);
	if (rpm > 800)
		return ms.GetVal(0,map(rpm,800,7500,0,11),map(s2.Temp(), 800, 7500, 0, 11));
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