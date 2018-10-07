// 
// Esta libreria junta todos los algoritmos de inyeccion en un solo archivo
// 

#include "InyecTime.h"
#include "Sensores.h"

//cuando tenga la libreria de memoria paso todo a variable, por ahora con define
//int INY_L = 150,   //tiempo de apertura del inyector en microsegundos
//INY_P = 500,   //tiempo en uS adicional para acelerar el motor
//INY_C = 25000; //Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiométrica (lambda=1 ) , para cilindrada del motor , presión a 100kPa , temperatura del aire a 21ºC y VE 100% .

#define INY_C 8000
#define INY_P 500
#define INY_L 150

/*-----( Variables _LMB )-----*/

#define LMBM  false //en true si se utilizan las dos sondas lambda
#define CTA   250 //Correcion de tiempo A, mezcla rica, se le sacan X uS
#define CTB   300 //Correcion de tiempo B, mezcla pobre, se le agregan X uS
#define P_LMB 250 //periodo en mS en el que se corrije por sonda lamba
#define T_LMB 45; //temperatura a partir de la cual se intenta correjir el tiempo de inyeccion
#define FLMBA 1.5 //factor maximo de lambda
#define FLMBB 0.85 //factor minimo de lambda

Sensores s(1);

unsigned long InyecTime::Tarr(){

	return 0;
}

unsigned long InyecTime::TLamb(unsigned long Tbase, byte temp){
	//tiempo de inyeccion
	float _LMB_XA = 800 * (3.3 / 1023); //medimos la sonda A
	float _LMB_XB = 0;
	//V menor a 0.45 = mezcla pobre
	if (_LMB_XA < 0.45) { //aca comparo por Volt pero tendriaaaa que hacerlo con los el factor lamba :S
		Tbase = Tbase + CTB;
	}
	else if (_LMB_XA > 0.45) { //V mayor a 0.45 = mezcla rica
		Tbase = Tbase - CTA;
	}
	if (LMBM) {
		_LMB_XB = 600 * (3.3 / 1023); //medimos la sonda B (Si corresponde)
									  //V menor a 0.45 = mezcla pobre
		if (_LMB_XB < 0.45) { //aca comparo por Volt pero tendriaaaa que hacerlo con los el factor lamba :S
			Tbase = Tbase + CTB;
		}
		else if (_LMB_XB > 0.45) { //V mayor a 0.45 = mezcla rica
			Tbase = Tbase - CTA;
		}
	}
	return Tbase;
}

unsigned long InyecTime::Tiny(byte vmar, byte rpm){
	return 0;
}

unsigned long InyecTime::Temp(byte vmar, byte rpm, byte temp){
	return 0;
}

unsigned long InyecTime::Aphplus(byte vmar, byte rpm2, byte temp, byte mapval, int VE[10][18]){
	/*
	PW = [INJ_CONST * VE(tps,rpm) * MAP * AirDensity] + AccEnrich +InjOpeningTime

	PW (pulse width) ---el tiempo final de apertura del inyector .

	INJ_CONST ----Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiométrica (lambda=1 ) , para cilindrada del motor , presión a 100kPa , temperatura del aire a 21ºC y VE 100% .

	VE para Speed Density (map, rpm)----Valor de eficiencia volumétrica tomada de la mapa VE .

	VE para ALPHA-N (tps, rpm)----Valor de eficiencia volumétrica tomada de la mapa VE .

	MAP(manifold absolute pressure)--- Presión tomada en el colector de admisión .

	AirDensity ---- Porcentual diferencia de densidad de aire comparada con la densidad de aire a temperatura 21ºC

	AccEnrich (acceleration enrichment) ---- Enriquecimiento de la mezcla en fase de aceleración .

	InjOpeningTime ---- Tiempo de apertura de inyector hasta el momento de inicio de inyección de combustible ( valor de retardo tomado de la mapa de calibración , INJECTORS CAL. )
	*/

	return ( (INY_C * (VE[map(mapval, 0, 150, 0, 10)][map(rpm2, 800, 6000, 0, 18)] / 100) ) * mapval * 10.1 ) + INY_P + INY_L;
}

unsigned long InyecTime::BPW(){
	/*
	BPW = BPC * MAP * T * A/F * VE * BVC * BLM * DFCO * DE * CLT * TBM
	BPW - Base Pulse Width
	BPC - Base Pulse Constant
	MAP - Manifold Absolute Pressure
	T - Temperature
	A/F - Air Fuel Ratio
	VE - Volumetric Efficiency
	BVC - Battery Voltage Correction
	BLM - Block Learn
	DFCO - Decel Fuel Cutoff
	DE - Decel Enleanment
	CLT - Closed Loop
	TBM - Turbo Boost Multiplier */
}
