// MAP.h

#ifndef _MAP_h
#define _MAP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
class TMAP {
public:
	TMAP(int pin, int pin2);//constructor de funcion
	float MasaAire();
	int PMA();
private:
	int _pin;
	int _pin2;
	int TMAP_Pm;
	int TMAP_PM;
	//pines sensor
	int pinTMAP[2];
	//medicion de presion
	int presion;
	float presionf;//presion
				   //medicion de temperatura :P
	float Vin;     // [V]       Tensión alimentación del divisor
	float Rfija;  // [ohm]     Resistencia fija del divisor
	float R25;    // [ohm]     Valor de NTC a 25ºC
	float Beta; // [K]      Parámetro Beta de NTC
	float T0;   // [K]       Temperatura de referencia en Kelvin
	float Vout;    // [V]       Variable para almacenar Vout
	float Rntc;    // [ohm]     Variable para NTC en ohmnios
	float TempK;   // [K]       Temperatura salida en Kelvin
	float R;//constante de gases ideales
	float mol; //moles del aire en(luego cambiar, varia por el tamaño del cuerpo de mariposa)
	float MA; //variable para guardar la Masa del aire que entra al cilindro, en L
};


#endif

