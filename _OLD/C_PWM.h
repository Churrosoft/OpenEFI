#include "Timer.h";
int PWM_FLAG_1 = 0;

//VARIABLES C_PWM
int AVC    = 0,
    PMSI   = 0,
	AVCI   = 0,
	FLAG_2 = 0,
	FLAG_3 = 0;
bool  V[8] = {false,false,false,false};
bool V2[8] = {false,false,false,false};
	
void int0(){
	PWM_FLAG_1++;
}

T Timer();

void C_PWM(int T){
	bool C1 = (FLAG_1 >= (PMSI - AVCI)); //Condicional para inyeccion
	bool C2 = (FLAG_1 >= (PMSI - AVC));  //Condicional para encendido
//*_*_*_*_*_*CONTROL INYECCION*_*_*_*_*_*_*
	if(C1){
		V[FLAG_2] = digitalRead(INY[FLAG_2]);
		digitalWrite(INY[FLAG_2],!V[FLAG_2]);
		delayMicroseconds(T);
		digitalWrite(INY[FLAG_2],V[FLAG_2]);
		FLAG_2++;
	}
	if(FLAG_2 >= 3){
		FLAG_2 = 0;
	}
//*_*_*_*CONTROL DE ENCENDIDO *_*_*_*_*_*_*_*_*_*_*
	if(C2){
		V[FLAG_3] = digitalRead(ECN[FLAG_3]);
		digitalWrite(ECN[FLAG_3],!V[FLAG_3]);
		delay(T);
		digitalWrite(ECN[FLAG_3],V[FLAG_3]);
		FLAG_3++;
	}
	if(FLAG_3 >= 3){
		FLAG_3 = 0;
	}
}