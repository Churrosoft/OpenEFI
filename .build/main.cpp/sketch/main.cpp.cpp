#line 1 "c:\\Users\\FDS2000\\OpenEFI\\main.cpp"
#line 1 "c:\\Users\\FDS2000\\OpenEFI\\main.cpp"
/*
╔════════════════════════════════════════════════════════════════════════╗
║                    Open EFI V 1.5X Team:                               ║
║  Main Programer:                                                       ║
║                  FDSoftware | dfrenoux@gmail.com | codereactor.xyz     ║
║  Co-Programer's                                                        ║
║                  FrancisJPK |                                          ║
║                  SFADalmiro | dalmiro.farinello99@gmail.com            ║
║ Program features:                                                      ║
║    ∆ Control of diesel / petrol engines up to 8 cylinders              ║ 
║    ∆ Single point and multipoint injection                             ║
║    ∆ Independent ignition                                              ║
║    ∆ Error report                                                      ║
║    ∆ Fixed injection / ignition mode                                   ║
║                                                                        ║
║                                                                        ║
║                                                                        ║  
╚════════════════════════════════════════════════════════════════════════╝
*/
/*-----( Importar Librerias )-----*/
#include <Arduino.h>
#include <Scheduler.h>
#include <LiquidCrystal_I2C.h>
#include <TMAP.h>
#include <variables.h>
#include <SPI.h>
#include <SD.h>
/*-----( Declarar variables :D )-----*/

#define motor 1 //definir tipo de motor 0 = diesel ; 1 = nafta
#define dev 1   //habilita mensajes por serial
String ver = "## ver 0.01 Alpha ##";
int cilindros = 4;   //cantidad de cilindros del motor
/*-----( Variable Globales )-----*/
//estas variables las puede usar cualquieeer funcion
int _RPM      = 0; //las rpm :V
int _TEMP     = 0; //temperatura
int _POS      = 0; //posicion del cigueñal (en dientes)
int _AE       = 0; //avance de encendido
void setup(){
	 #if(dev == 1)
		 Serial.begin(9600);
	 #endif
	 Scheduler.startLoop(HILO_1);
	 Scheduler.startLoop(HILO_2);
	 Scheduler.startLoop(HILO_3);
}

void loop(){
	//este hilo se encarga de la comunicacion con pc y programacion
}

void HILO_1(){
	//RPM / Temp / Sensores
}

void HILO_2(){
	//C INY / Fixed Mode
}

void HILO_3(){
	//control encendido
}

int sinc(int op, int dato){ //esta funcion sincroniza todaaas las variables globales
	switch(op){
		case 0:
			_RPM = dato;
			break;
		case 1:
			_TEMP = dato;
			break;
		case 2:
			_POS = dato;
			break;
		case 3:
			_AE = dato;
			break;
		case 4:
			return(_RPM);
			break;
		case 5:
			return(_TEMP);
			break;
		case 6:
			return(_POS);
			break;
		case 7:
			return(_AE);
			break;
	}
	return(0);
}
