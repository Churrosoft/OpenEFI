#ifndef _C_PWM_h
#define _C_PWM_h
#include "defines.h"
class C_PWM{

public:

	C_PWM(uint8_t pinesE[], uint8_t pinesI[]); //bob el construcctor de funciones
	void Intr();
	void Iny();
	uint8_t AVC = 15; //avance encendido
	unsigned long 	pT1 = 120,    //tiempo de inyeccion combustible
					pT2 = 1800;   //tiempo de encendido bobina
private:
	
	unsigned int PWM_FLAG_1  = 0;
	unsigned int PWM_FLAG_1A = 0;
	uint8_t
		PWM_FLAG_2  = 0,
		PWM_FLAG_3  = 0,
		i = 0;       //para for viteh.
	uint8_t INY[CIL];
	uint8_t ECN[CIL];
	unsigned long    //temporizados:
		Time = 0,
		T1X = 0,	 //lo uso para controlar temporizado con micros()
		T2X = 0;	 //idem pero para encendido
	bool t1 = false,
	     t2 = false; //bool para control de temperizado

};
#endif