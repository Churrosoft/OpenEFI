// PWM.h

#ifndef _PWM_h
#define _PWM_h
#include "arduino.h"
class SPWM{

public:
	//avance encendido  | dientes entre pms | pines inyeccion | pines encendido
	SPWM(byte avance,      byte dientesPMS,   byte PinesI[],    byte PinesE[]);
	void Ecn(byte Time, byte Avc);
	void Set(unsigned long _Time);
	void Iny();
	void Intrr();    //void de interrupcion
private:
	/*-----( Variables C_PWM )-----*/
	byte
		PMSI = 30,   //Cantidad de dientes entre PMS
		AVCI = 10,   //Avance de inyeccion
		AVC = 15,    //Avance de encendido
		PWM_FLAG_1  = 0,
		PWM_FLAG_1A = 0,
		PWM_FLAG_2  = 0,
		PWM_FLAG_3  = 0,
		i = 0;       //para for viteh.
	byte INY[16];
	unsigned long    //temporizados:
		Time = 0,
		T1 = 250,    //tiempo de inyeccion combustible
		T2 = 1800,   //tiempo de encendido bobina
		T1X = 0,	 //lo uso para controlar temporizado con micros()
		T2X = 0;	 //idem pero para encendido
	bool t1 = false,
	     t2 = false; //bool para control de temperizado
};

#endif