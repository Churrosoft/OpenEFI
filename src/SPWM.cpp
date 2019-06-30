// 
// Esta wea se encarga del PWM para inyectores y bobinas, nada mas ?)
// Proximamente una libreria aparte para cuando el motor es diesel
// 

#define cil 4   //cantidad de cilindros del motor

#include "SPWM.h"
SPWM::SPWM(byte avance, byte dientesPMS, byte PinesI[] , byte PinesE[]){
	AVCI = avance;
	PMSI = dientesPMS;
	for (i = 0; i < sizeof(PinesI);i++) {
		pinMode(PinesI[i], OUTPUT);
		pinMode(PinesE[i], OUTPUT);
	}
	//DDRB |= (1 << DDRB2); "SPWM.cpp: 16:16: error: 'DDRB2' was not declared in this scope"
}

void SPWM::Ecn(byte Time, byte Avc){
#ifdef mtr == 1
#ifdef uEFI == 0 && OpenEFImode == 0

	if (PWM_FLAG_1A >= (PMSI - AVC)) {
#if defined(ESP8266)
		digitalWrite(ECN[PWM_FLAG_3], !digitalRead(INY[PWM_FLAG_3]));
#endif
#if defined(__AVR_ATmega328P__)
		PORTD |= (0 << ECN[PWM_FLAG_3]); //thanks isa :3
#endif
		t2 = true;
	}

	if (micros() - T2X >= Time && t2 == true) {
#if defined(ESP8266)
		digitalWrite(ECN[PWM_FLAG_3], !digitalRead(INY[PWM_FLAG_3]));
#endif
#if defined(__AVR_ATmega328P__)
		PORTD |= (1 << ECN[PWM_FLAG_3]);
#endif
		PWM_FLAG_3++;
		PWM_FLAG_1A = 0;
		if (PWM_FLAG_3 > (cil - 1)) PWM_FLAG_3 = 0; //remplazar luego con el define "cil"
		T2X = micros();
		t2 = false;
	}
#endif // uEFI == 0 && OpenEFImode == 0
#endif // mtr == 1
}

void SPWM::Set(unsigned long _Time) {
	Time = _Time; //nada mas ?)
}

void SPWM::Iny(){ //esto va en el loop principal
#if mtr == 1 || mtr == 0
#if uEFI == 0 && OpenEFImode == 0
	if (PWM_FLAG_1 >= (PMSI - AVCI)) {
#if defined(ESP8266)
		digitalWrite(INY[PWM_FLAG_2], !digitalRead(INY[PWM_FLAG_2]));
#endif
#if defined(__AVR_ATmega328P__)
		PORTD |= (1 << INY[PWM_FLAG_2]);
#endif
		t1 = true;
	}

	if (micros() - T1X >= Time && t1 == true ) {
#if defined(ESP8266)
		digitalWrite(INY[PWM_FLAG_2], !digitalRead(INY[PWM_FLAG_2]));
#endif
#if defined(__AVR_ATmega328P__)
		PORTD |= (0 << INY[PWM_FLAG_2]);
#endif
		PWM_FLAG_2++;
		PWM_FLAG_1 = 0; //reseteo para proximo tiempo
		if (PWM_FLAG_2 > (cil - 1)) PWM_FLAG_2 = 0;
		t1 == false;
		T1X = micros();
	}
#endif // uEFI == 0 && OpenEFImode == 0
#endif
}

void SPWM::Intrr() {
	//este void controla la interrupcion
	PWM_FLAG_1++; //con esto me fijo donde corno esta el cigueñal
#if mtr == 1
	PWM_FLAG_1A++;
#endif // mtr == 1
}

//Control de PWM antiguo
//void C_PWM() {
//	bool C1 = (PWM_FLAG_1 >= (PMSI - AVCI)); //Condicional para inyeccion
//#if mtr == 1
//	bool C2 = (PWM_FLAG_1A >= (PMSI - AVC));  //Condicional para encendido
//#endif
//	//*_*_*_*_*_*CONTROL INYECCION*_*_*_*_*_*_*
//	if (C1) {
//		digitalWrite(INY[PWM_FLAG_2], !digitalRead(INY[PWM_FLAG_2]));
//		delayMicroseconds(INYT1);
//		digitalWrite(INY[PWM_FLAG_2], !digitalRead(INY[PWM_FLAG_2]));
//		PWM_FLAG_2++;
//		PWM_FLAG_1 = 0;
//	}
//	if (PWM_FLAG_2 > (cil - 1)) {
//		PWM_FLAG_2 = 0;
//	}
//	//*_*_*_*CONTROL DE ENCENDIDO *_*_*_*_*_*_*_*_*_*_*
//#if mtr == 1
//	if (C2) {
//		digitalWrite(ECN[PWM_FLAG_3], !digitalRead(ECN[PWM_FLAG_3]));
//		delayMicroseconds(INYT2);
//		digitalWrite(ECN[PWM_FLAG_3], !digitalRead(ECN[PWM_FLAG_3]));
//		PWM_FLAG_3++;
//		PWM_FLAG_1A = 0;
//	}
//	if (PWM_FLAG_3 > (cil - 1)) {
//		PWM_FLAG_3 = 0;
//	}
//#endif
//}