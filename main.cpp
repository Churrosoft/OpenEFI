//Programa principal
//Creado por FDSoftware/Frenoux Diego

#include <Arduino.h>

int ord_enc[] = {0, 2 ,3 ,1};//orden de encendido motor
int iny[] = {2,3,4,5};       //Pines de arduino que esta conectados los inyectores
int tMax     = 22;           //tiempo maximo de inyeccion

int pul  = LOW;             //pulso de disparo de sensor de posicion de cigueñal
bool varrpm = LOW;          //variable para no volver a contar mismo diente como pulso
int pinrpm = 10;            //pin conectado al sensor de posicion de cigueñal

int diente = 0;             //numero de diente actual de la corona
int intervalo1 = 500;       //intervalo para medir rpm
float inyT = 1.5;              //tiempo de inyeccion ms
int esperaINY = 500;      //tiempo que espera inyeccion para ver si el motor acelera


int Edef  = XX;             //cantidad de grados de avance del encendido con motor a temperatura funcionamiento
int EFRIO = XX;             //cantidad de grados de avance del encendido con motor frio
int EARR  = XX;             //cantidad de grados de avance del encendido con motor en modo arranque

int Mar   = AN0;            //pin de mariposa de acelerador
int marv  = 0;              //valor de mariposa de acelerador
int Lamb  = AN1;            //pin de sonda lambda

int var = 0;                //variable usada para bucles
int varX = 0;               //variable temporal, multiples usos
int varINY = 0              //varible temporal, guarda ultimo inyector activado

int rpm    = 0;                //numero actual de rpm
int rpmMIN = 800;             //numero minimo de rpm
int rpmMAX = 7000;            //numero maximo de rpm

int vuelta;                    //numero de vuelta para rpm
bool varr = false;             //variable para evitar calcular rpm varias veces
int vuelta2;                   //numero de vuelta para inyeccion/encendido
bool inyec = false;            //variable para saber si se esta inyectando todavia o no
bool acelerar;                 //varible temporar, guarda si se intento acelerar las rpm o no
unsigned long prevMillis; //tiempo previo ultimo periodo medicion rpm
unsigned long curMillis; //tiempo actual del micro

void setup(){
    //definir E/S
    pinMode(pinrpm, INPUT);

    for(var = 0, var >= 3, var++;){
        pinMode(iny[var], OUTPUT);
    }

    for(var = 0, var >= 3, var++;){
        digitalWrite(iny[var], LOW);
    }
    //definir interrupciones HW
    attachInterrupt(0,int0,RISING);
}

void loop(){
    
        Temperatura();
        ControlRPM();
        ControlINY();
        ControlENC();
        ControlPWM();
}

void ControlRPM(){    
   //calcula rpm cada 1 vuelta del cigueñal
      if(varr == false && vuelta == 0){
	  milisant = millis();
	  varr = true;
     }
  
     if(varr == true && vuelta == 2){
	    rpm = (millis() - milisant) * 60000;
	    varr = false;
	    var2 = 0;
     }
}

void ControlINY(){
    //Controla los tiempos de inyeccion dependiendo la mariposa,rpm y temperatura del motor
    if(rpm <= rpmMIN && varm <= 10 && acelerar == false){ //si las rpm caen debajo las rpm minimas y no esta presionado acelerador
            inyT = intT + 0.5;
            acelerar = true;
            rpmant = rpm;
         }

         if(rpm >= rpmMIN && varm <= 10 && acelerar == false){ //si las rpm pasan las rpm minimas y no esta presionado acelerador
            inyT = intT - 0.5;
            acelerar = true;
            rpmant = rpm;
         }
 int varxx = rpm - rpmant;
    if(vuelta3 >= 20 && varxx <= 5){
        // si no hubo un cambio mayor a 5 rpm luego de 10 vueltas de cigueñal volver a corregir
        acelerar = false;
        vuelta3 = 0;
    }
    
}
void Control PWM{
    if (inyectando == false){
	digitalWrite(iny[vuelta2++],HIGH);
	inyectando = true
}

	if (millis() - millis >= inYT) {
		digitalWrite(iny[vuelta2++],LOW);
		if(vuelta2 == 3){
			vuelta2 == 0;
		}
		inyectando == false
	}
}


void ControlENC(){
    //Controla el encendido dependiendo de la posicion del cigueñal y temperatura
}

void Temperatura(){
    //Controla el ventilador y apagado de emergencia del motor
}

int0{
    vuelta++;
    vuelta2++;
    vuelta3++;
    varr = true;
}