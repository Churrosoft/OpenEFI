//############################################
//#####Libreria de control de sensor TMAP#####
//###########por FDSoftware###################
//############################################
#ifndef TMAP_h
#define TMAP_h


#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include <math.h>
#include <TMAP.h>

class TMAP{
	public:
		TMAP_in(int pin, int pin2, int map, int map2);//constructor de funcion
	private:
	int TMAP_Pm;
	int TMAP_PM;
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
	float TempC;   // [ºC]      Temperatura salida en Celsius
	float R;//constante de gases ideales
	float mol; //moles del aire en(luego cambiar, varia por el tamaño del cuerpo de mariposa)
}

#endif