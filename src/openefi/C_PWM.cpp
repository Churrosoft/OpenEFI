// 
// Esta wea se encarga del PWM para inyectores y bobinas, nada mas ?)
// Proximamente una libreria aparte para cuando el motor es diesel
// 
#include "C_PWM.h"

C_PWM::C_PWM(int pinesE[], int E_PORT, int pinesI[], int I_PORT){
    for (i = 0; i < CIL;i++) {
		INY[i] = pinesI[i];
		ECN[i] = pinesE[i];
		pinMode(I_PORT, pinesI[i], true);
		pinMode(E_PORT, pinesE[i], true);
	}
}

void C_PWM::Intr(){
	//este void controla la interrupcion
	PWM_FLAG_1++; //con esto me fijo donde corno esta el cigueñal
#if mtr == 1
	PWM_FLAG_1A++;
#endif // mtr == 1
	//Luego revisar si hacer esto no rompe el universo
	//this->Iny();
	//this->Ecn();

}

void C_PWM::Iny(){
	if (PWM_FLAG_1 >= (PMSI - AVCI) && t1 == false) {
		//digitalWrite(INY[PWM_FLAG_2], HIGH);
		t1 = true;
		T1X = micros();
	}
	Time = micros();

	if ((Time - T1X) >= pT1 && T1X != 0 && t1 == true) {
		//digitalWrite(INY[PWM_FLAG_2], LOW);
		PWM_FLAG_2++;
		PWM_FLAG_1 = 0; //reseteo para proximo tiempo
		if (PWM_FLAG_2 > (CIL - 1)) PWM_FLAG_2 = 0;
		t1 = false;
	}
}

void C_PWM::Ecn(){

	if (PWM_FLAG_1A >= (PMSI - AVC) && t2 == false) {
		//digitalWrite(ECN[PWM_FLAG_3], HIGH);
		t2 = true;
		T2X = micros();
	}
	Time = micros();

	if ((Time - T2X) >= ECNT && T2X != 0 && t2 == true) {
		//digitalWrite(ECN[PWM_FLAG_3], LOW);
		PWM_FLAG_3++;
		PWM_FLAG_1A = 0; //reseteo para proximo tiempo
		if (PWM_FLAG_3 > (CIL - (CIL / 2) - 1) ) PWM_FLAG_3 = 0; 
		//paso a explicar antes que me linches, con esto termino teniendo un 1 en un motor de 4 cilindros, porque solo hay dos bobinas, (una cada dos cilindros)
		t2 = false;
	}

}