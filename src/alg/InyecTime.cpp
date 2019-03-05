// 
// Esta libreria junta todos los algoritmos de inyeccion en un solo archivo
// 

#include "InyecTime.h"

//cuando tenga la libreria de memoria paso todo a variable, por ahora con define
//int INY_L = 150,   //tiempo de apertura del inyector en microsegundos
//INY_P = 500,   //tiempo en uS adicional para acelerar el motor
//INY_C = 25000; //Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiom�trica (lambda=1 ) , para cilindrada del motor , presi�n a 100kPa , temperatura del aire a 21�C y VE 100% .

#define INY_C 8000
#define INY_P 500
#define INY_L 150
#define ARRTp 45 //temperatura para definir si es arranque en frio o en caliente
#define TARR  25000 //tiempo de inyeccion para arranque (frio)
#define TARR2 2500 //tiempo de inyeccion para arranque (caliente)
/*-----( Variables _LMB )-----*/

#define LMBM  false //en true si se utilizan las dos sondas lambda
#define CTA   250 //Correcion de tiempo A, mezcla rica, se le sacan X uS
#define CTB   300 //Correcion de tiempo B, mezcla pobre, se le agregan X uS
#define P_LMB 250 //periodo en mS en el que se corrije por sonda lamba
#define T_LMB 45 //temperatura a partir de la cual se intenta correjir el tiempo de inyeccion
#define FLMBA 1.5 //factor maximo de lambda
#define FLMBB 0.85 //factor minimo de lambda

/*-----( Variables BPW )-----*/

#define BPC 1500  //Base Pulse Constant
#define AF 123 // relacion aire combustible, se divide por 10 para no usar float
#define BVC 1 //correcion por bateria, luego intentar usar tabla
#define BLM 168 // "Block Learn" varia entre 168 y 170 dependiendo de como resultaron los tiempos
//anteriores, seudo IA para mejorar el tiempo de inyeccion
#define DFCO 1 //ni puta idea
#define DE 1 // ni puta idea x2
#define TBM 1 //turbo boost multiplier

//Libreria para generar tiempos de inyeccion
InyecTime::InyecTime(Sensores& s1, Memory& mI){
	//por ahora no hace falta iniciar mas cosas aca
	CLT = 1.3; //correccion de lambda, pero en vez de sumar o restar multiplico por X valor para obtener el tiempo)
}
unsigned long InyecTime::Tarr(){
	if (s1.Temp() >= ARRTp) {
		return TARR2;
	}
	return TARR;
}

unsigned long InyecTime::TLamb(unsigned long Tbase, byte temp){
	//tiempo de inyeccion
	float _LMB_XA = s1.lamb(0); //medimos la sonda A
	float _LMB_XB = 0;
	//V menor a 0.45 = mezcla pobre
	//TODO: cambiar Volt a factor lambda
	if (_LMB_XA < 0.45) {
		Tbase = Tbase + CTB;
	}
	else if (_LMB_XA > 0.45) { //V mayor a 0.45 = mezcla rica
		Tbase = Tbase - CTA;
	}
	if (LMBM) {
		_LMB_XB = s1.lamb(1); //medimos la sonda B (Si corresponde)
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

unsigned long InyecTime::TTable(int rpm){
	return mI.GetVal(1, rpm, s1.tps());
}

float InyecTime::VtoLamb(float v) {
	//convierte volts en factor lambda
}
unsigned long InyecTime::Temp(int rpm){
	//UNDONE: correccion por temperatura
	//Correccion de tiempo por temperatura (tendria que fijarme en la tabla)
	return 0;
}

unsigned long InyecTime::Aphplus(int rpm2){
	/*
	PW = [INJ_CONST * VE(tps,rpm) * MAP * AirDensity] + AccEnrich + InjOpeningTime

	PW (pulse width) ---el tiempo final de apertura del inyector .

	INJ_CONST ----Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiom�trica (lambda=1 ) , para cilindrada del motor , presi�n a 100kPa , temperatura del aire a 21�C y VE 100% .

	VE para Speed Density (map, rpm)----Valor de eficiencia volum�trica tomada de la mapa VE .

	VE para ALPHA-N (tps, rpm)----Valor de eficiencia volum�trica tomada de la mapa VE .

	MAP(manifold absolute pressure)--- Presi�n tomada en el colector de admisi�n .

	AirDensity ---- Porcentual diferencia de densidad de aire comparada con la densidad de aire a temperatura 21�C

	AccEnrich (acceleration enrichment) ---- Enriquecimiento de la mezcla en fase de aceleraci�n .

	InjOpeningTime ---- Tiempo de apertura de inyector hasta el momento de inicio de inyecci�n de combustible ( valor de retardo tomado de la mapa de calibraci�n , INJECTORS CAL. )
	*/
	int ve = mI.GetVal( 2, map( s1.tps(), 0, 150, 0, 10 ), map (rpm2, 800, 6000, 0, 18 ) );

	return ( (INY_C * (ve / 100) ) * s1.pres() * 10.1 ) + INY_P + INY_L;
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
	BVC - Battery Voltage Correction (sacar de tablas)
	BLM - Block Learn (default 168-170)
	DFCO - Decel Fuel Cutoff (ni puta idea)
	DE - Decel Enleanment (ni puta idea x2)
	CLT - Closed Loop (correccion de lambda, 
	pero en vez de sumar o restar multiplico por X valor para obtener el tiempo )
	TBM - Turbo Boost Multiplier */
	return BPC * s1.tps() * s1.Temp() * AF * BVC * BLM * DFCO * DE * CLT * TBM;
}
