// 
// Esta wea se encarga del PWM para inyectores y bobinas, nada mas ?)
// Proximamente una libreria aparte para cuando el motor es diesel
// 
#include "C_PWM.h"

C_PWM::C_PWM(uint8_t pinesE[], uint8_t pinesI[]){
    for (i = 0; i < CIL;i++) {
		pinMode(pinesI[i], OUTPUT);
		pinMode(pinesE[i], OUTPUT);
		digitalWrite(pinesI[i], LOW);
		digitalWrite(pinesE[i], LOW);
		INY[i] = pinesI[i];
	}
}

void C_PWM::Intr(){
	//este void controla la interrupcion
	PWM_FLAG_1++; //con esto me fijo donde corno esta el cigueñal
#if mtr == 1
	PWM_FLAG_1A++;
#endif // mtr == 1
	//this->Iny();

}

void C_PWM::Iny(){
	if (PWM_FLAG_1 >= (PMSI - AVCI) && t1 == false) {
		digitalWrite(INY[PWM_FLAG_2], HIGH);
		t1 = true;
		T1X = micros();
	}
	Time = micros();

	if ((Time - T1X) >= pT1 && T1X != 0 && t1 == true) {
		digitalWrite(INY[PWM_FLAG_2], LOW);
		PWM_FLAG_2++;
		PWM_FLAG_1 = 0; //reseteo para proximo tiempo
		if (PWM_FLAG_2 > (CIL - 1)) PWM_FLAG_2 = 0;
		t1 = false;
	}
}