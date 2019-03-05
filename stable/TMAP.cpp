// 
// Libreria TMAP.cpp reformada (para sensor i2c BMP180 y sensor de temperatura de aire generico)
// 

#include "TMAP.h"
#define R 0.08     //constante de gases ideales
#define mol 0.625  //moles del aire en(luego cambiar, varia por el tamaño del cuerpo de mariposa)
#define Rfija 1000 // [ohm]     Resistencia fija del divisor
#define R25 2800   // [ohm]     Valor de NTC a 25ºC
#define Beta 3900  // [K]      Parámetro Beta de NTC
#define T0 293.15  // [K]       Temperatura de referencia en Kelvin
#define Pm 12      // presion minima en Kpa que puede medir el sensor MAP
#define PM 120     // presion maxima en Kpa que puede medir el sensor MAP

TMAP::TMAP(CD4051& mux){
	mux.GetVal(0);
}

float TMAP::MasaAire() {
	//medicion de presion (aproximado)
	presionf = map(mux.GetVal(6),0,1023,Pm, PM);
	//Calculamos la temperatura
	Vout = (5 / 1024)*(mux.GetVal(2));
	//resistencia de la NTC
	Rntc = (Vout*Rfija) / (5 - Vout);
	//temperatura en Kelvin
	TempK = Beta / (log(Rntc / R25) + (Beta / T0));
	//calculamos la mase del aire
	MA = (mol* R *TempK) / (presionf / 101,325); //presionf / 101,325 es para pasar de Kpa a atm
	return MA; //Devolvemos la masa de aire que entra en la admision en litros
}

int TMAP::PMA() { //Presion multiple admision

	return 0; map(mux.GetVal(6), 0, 1023, Pm, PM);
}