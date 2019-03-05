// 
//  DTC.cpp, esta libreria vigila los sensores y crea dtc's
// 
#include "DTC.h"

//mega bloque de defines con rango de valores para los sensores, a futuro se guarda en eeprom
// MIN = valor minimo en caso que el sensor funcione
// MAX = valor maximo en caso que el sensor funcione

#define tpsMIN  15  //0.15v
#define tpsMAX  450 //4.5v
#define tmpMIN  15
#define tempMAX 450
#define TairMIN 12 //temperatura aire
#define TairMAX 134
#define lmbMIN  10 //sondas lambda
#define lmbMAX  300
#define PresMIN 10 //sensor de presion de aire
#define PresMAX 210
//para motores con mariposa motorizada (por solenoide no paso a paso)
#define mar1MIN 14 //Mariposa de acerelador "A", tps funciona como mariposa "B" en este modo
#define mar1MAX 110
#define acl1MIN 123//sensor de posicion de pedal de acerelador (unico potenciometro), luego agrego 2
#define acl1MAX 213

//Define de codigos DTC:
#define DTC_TPSA   0122 //voltaje inferior a 0.15v 
#define DTC_TPS    0121 //voltaje superior a 4.5v o inferior a 0.15v
#define DTC_LAMB_A 0135 //error en sonda lambda A
#define DTC_LAMB_B 0141 //error en sonda lambda B
#define DTC_AIR    0113 //error en sensor de temperatura de aire
#define DTC_Pres   0106 //error en sensor de presion de aire

DTC::DTC( Memory & mem2, Sensores & s3 ){
}

void DTC::DTC_Loop(){

	if (s3.tps(true) < tpsMIN || s3.tps(true) > tpsMAX)//repetir hasta que me quede sin sensores(o ganas de vivir)
		mem2.SetDTC(DTC_TPS);
	if (s3.lamb(0, true) < lmbMIN || s3.lamb(0,true) > lmbMAX)
		mem2.SetDTC(DTC_LAMB_A);
	if (s3.lamb(1, true) < lmbMIN || s3.lamb(1, true) > lmbMAX)
		mem2.SetDTC(DTC_LAMB_B);
	if (s3.airTemp(true) < TairMIN || s3.airTemp(true) > TairMAX)
		mem2.SetDTC(DTC_AIR);
	if (s3.pres(true) < PresMIN || s3.pres(true) > PresMAX)
		mem2.SetDTC(DTC_Pres);
}