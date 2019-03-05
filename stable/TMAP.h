// MAP.h

#ifndef _MAP_h
#define _MAP_h
#include "CD4051.h"
class TMAP{
public:
	CD4051& mux;
	TMAP(CD4051& mux);//constructor de funcion
	float MasaAire();
	int PMA();
private:
	//pines sensor
	//medicion de presion
	float presionf;//presion
	float Vout;
	int Rntc;    // [ohm]     Variable para NTC en ohmnios
	float TempK; // [K]       Temperatura salida en Kelvin
	float MA;    //variable para guardar la Masa del aire que entra al cilindro, en L
};
#endif