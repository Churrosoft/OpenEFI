/*
╔════════════════════════════════════════════════════════════════════════╗
║                    Open EFI V 1.5X Team:                               ║
║  Main Programer   FDSoftware | dfrenoux@gmail.com | fdsoftware.xyz     ║
║  Ex Co-Programer's                                                     ║
║                  FrancisJPK |                                          ║
║                  SFADalmiro | dalmiro.farinello99@gmail.com            ║
║ Program features:                                                      ║
║    ∆ Control of diesel / petrol engines up to 8 cylinders              ║
║    ∆ Single point and multipoint injection                             ║
║    ∆ Independent ignition                                              ║
║    ∆ Error report                                                      ║
║    ∆ Fixed injection / ignition mode                                   ║
╚════════════════════════════════════════════════════════════════════════╝
*/

/*-----( Importar Librerias )-----*/
#include "PWM.h"
#include <Arduino.h>
#include <Wire.h>
#include <Sensores.h>
#include <variables.h>

/*-----( Inicio de librerias )-----*/
Sensores S(1);

/*-----( Define's )-----*/

#define mtr 1   //definir tipo de motor 0 = diesel ; 1 = nafta
#define dev 1   //habilita modo desarollo
#define alpha 1 //Habilita el modo de prubas alpha
#define cil 4   //cantidad de cilindros del motor
#define VM 1    //modo del ventialdor, 1 = fijo, a partir de x temperatura se prende, 0 = control de velocidad variable por pwm

void setup() {
	Wire.begin();
	Serial.begin(115200);
	Serial.println("DBG Iniciando");
	Serial.println("LMB 1.2");
	Serial.println("00V 12.45");
	Serial.println("INY 3.82");
	Serial.println("AVC 12.3");
	attachInterrupt(digitalPinToInterrupt(2), I_RPM, CHANGE);
	Serial.print("DBG Tarde: ");
	Serial.print(millis());
	Serial.println("mS");
	Serial.println("LMB 0");
	Serial.println("00V 0");
	Serial.println("INY 0");
	Serial.println("AVC 0");
	//analogWriteResolution(12);
}

void loop() {

}

void I_RPM() { //interrupcion para rpm
	_PR++;
	_POS++;
	if (tdnt == 0) { Tdnt[tdnt] = micros(); tdnt++; }// para guardar primer temporizado del sincronizado
	Tdnt[tdnt] = micros() - Tdnt[(tdnt - 1)];
	if (_POS >= dnt) {
		//if (!SINC) { sincronizar(); }
		_POS = 0;
	}
	PWM_FLAG_1++;
	PWM_FLAG_1A++;
}