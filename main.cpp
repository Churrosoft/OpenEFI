//Programa principal
//Creado por FDSoftware/Frenoux Diego

#include <Arduino.h>

int ord_enc[] = 0, 2 ,3 ,1; //orden de encendido motor
int iny[] = 2,3,4,5;       //Pines de arduino que esta conectados los inyectores
int tRalenti = 2;          //tiempo de inyeccion en ralenti
int tMax     = 22;          //timpo maximo de inyeccion

int pul  = LOW;             //pulso de disparo de sensor de posicion de cigueñal
bool varrpm = LOW;          //variable para no volver a contar mismo diente como pulso
int pinrpm = 10;            //pin conectado al sensor de posicion de cigueñal

int diente = 0;             //numero de diente actual de la corona
int intervalo1 = 500;       //intervalo para medir rpm

int dnt  = XX;              //cantidad de dientes de corona
int dntI = XX;              //cantidad de dientes entre inyecciones
int dntE = XX;              //cantidad de dientes entre punto muerto superior de cilindros

int Edef  = XX;             //cantidad de grados/dientes de avance del encendido con motor a temperatura funcionamiento
int EFRIO = XX;             //cantidad de grados/dientes de avance del encendido con motor frio
int EARR  = XX;             //cantidad de grados/dientes de avance del encendido con motor en modo arranque

int Mar   = AN0;            //pin de mariposa de acelerador
int Lamb  = AN1;            //pin de sonda lambda

int var = 0;                //variable usada para bucles
int rpm = 0;
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
}

void loop(){
    
        Temperatura();
        ControlRPM();
        ControlINY();
        ControlENC();
}

void ControlRPM(){    
    //cada vez que detecta un pulso del sensor de posicion de cigueñal, miede las rpm, y guarda
    //la cantidad de dientes que va detectando

    //Detectamos pulsos y guardamos dato
     puls = digitalRead(IN1);
     if (puls == LOW && var == false){
         diente++;
        varrpm = true;
    }if(puls == HIGH && var6[var5] == true){
     varrpm = false;
  }
    //calculo RPM
     curMillis = millis();
      if (curMillis - prevMillis >= intervalo1) {
	    	prevMillis = curMillis;
	    	if(diente <= dnt){
                rpm = ((diente * 2)*60 /dnt);
		    }
      }

if (diente == dnt){
	diente = 0;
}

}

void ControlINY(){
    //Controla los tiempos de inyeccion dependiendo la mariposa,rpm y temperatura del motor
}

void ControlENC(){
    //Controla el encendido dependiendo de la posicion del cigueñal y temperatura
}

void Temperatura(){
    //Controla el ventilador y apagado de emergencia del motor
}