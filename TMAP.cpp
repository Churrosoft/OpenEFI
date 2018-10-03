// 
// Libreria TMAP.cpp reformada
// 

#include "TMAP.h"

TMAP::TMAP(int pin, int pin2) {
	pinMode(pin, INPUT);
	pinMode(pin2, INPUT);
	pinTMAP[0] = pin;
	pinTMAP[1] = pin2;
}

float TMAP::MasaAire() {
	//medicion de presion
	presion = 0;
	presionf = 0.00; //presion
	R = 0.08;//constante de gases ideales
	mol = 0.625; //moles del aire en(luego cambiar, varia por el tamaño del cuerpo de mariposa)
				 //medicion de temperatura :P
	Vin = 5.0;     // [V]       Tensión alimentación del divisor
	Rfija = 1000;  // [ohm]     Resistencia fija del divisor
	R25 = 2800;    // [ohm]     Valor de NTC a 25ºC
	Beta = 3900.0; // [K]      Parámetro Beta de NTC
	T0 = 293.15;   // [K]       Temperatura de referencia en Kelvin
	Vout = 0.0;    // [V]       Variable para almacenar Vout
	Rntc = 0.0;    // [ohm]     Variable para NTC en ohmnios
	TempK = 0.0;   // [K]       Temperatura salida en Kelvin	

				   //Calculamos la temperatura
	Vout = (Vin / 1024)*(analogRead(pinTMAP[1]));
	//resistencia de la NTC
	Rntc = (Vout*Rfija) / (Vin - Vout);
	//temperatura en Kelvin
	TempK = Beta / (log(Rntc / R25) + (Beta / T0));
	//Medimos la presion (no creo que funcione muy bien que digamos XD)
	presion = analogRead(pinTMAP[0]);
	presion = map(presion, 0, 1023, 145, 210); //145 = 1,45 psi presion minima, 210 = 2,10 psi presion maxima
	presionf = presion / 100;
	//calculamos la mase del aire
	MA = (mol* R *TempK) / (0.07 * presionf); //0.7 * presionf es para pasar de PSI a atm
	return MA; //Devolvemos la masa de aire que entra en la admision en litros
}

int TMAP::PMA() { //Presion multiple admision
				  //Medimos la presion (no creo que funcione muy bien que digamos XD)
	presion = analogRead(pinTMAP[0]);
	presion = map(presion, 0, 1023, 145, 210); //145 = 1,45 psi presion minima, 210 = 2,10 psi presion maxima
	presionf = presion / 100;
	return presionf;
}