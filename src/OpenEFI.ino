#include <Arduino.h>
#include <Wire.h>

#include "defines.h"
#include "variables.h"
#include "C_PWM.h"

#include "interfazSerial.h"
#include "interfazCAN.h"

// Inicio de librerias
interfazSerial Ser;
C_PWM MyPWM(5);
void setup(){
    Ser = interfazSerial();
    Ser.send(F("MSG"), F("Starting up"));
    Ser.send(F("VER"), 200);
    // TODO
}

void loop(){
    // TODO
    Ser.query();
	RPM();
}

void RPM(){ //void boludo para calcular las rpm, recuperado de las profundidades del git
  T_RPM_AC = millis();
  if (T_RPM_AC - T_RPM_A >= RPM_per) {
         T_RPM_A = T_RPM_AC;
         _RPM = (_POS / DNT)*90; //calculo para obtener las rpm
         _POS = 0;
    }

}
#if Alpha == 1
//interrupcion para rpm (SIN SINCRONIZAR PMS NO USAR EN UN MOTOR O ROMPE' TODO)
void I_RPM() { 
	_POS++;
	MyPWM.Intr();
}
#endif
#if Alpha == 0
void I_RPM() { //interrupcion para rpm
	if (SINC) {
		_POS++;
		MyPWM.Intr();
	}
	if (!SINC)
		sincINT();
}

void sincINT() {
	//interruppcion para "sincronizar()"
	if (!initsinc) {
		Ta = micros();
		initsinc = true;
	}
	else {
		if (sincB) {
			Tb = micros();
			T1 = Tb - Ta;
			Ta = Tb;
			sincB = false;
		}
		else {
			Tb = micros();
			T2 = Tb - Ta;
			Ta = Tb;
			sincB = true;
			sincronizar();
		}
	}
}
void sincronizar() {
	//este void caza el ultimo tiempo entre diente y si es mayor por 1.5 veces al anterior,
	//marca que es el PMS
	 if ( T2 > ( T1 + (T1 / 2.3 ) )  ) {
		SINC = true;
	}
}
#endif