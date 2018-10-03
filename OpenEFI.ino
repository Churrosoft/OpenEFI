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

/*-----( Define's )-----*/
#define mtr 1   //definir tipo de motor 0 = diesel ; 1 = nafta
#define dev 1   //habilita modo desarollo
#define alpha 1 //Habilita el modo de prubas alpha
#define GNC_MOD 0 //activa el modo de GNC/GLP en la inyeccion
#define uEFI 0 //en 1 si se compila para una placa uEFI
#define OpenEFImode 0 // en 0 si es OpenEFI con Arduino nano, en 1 si utiliza un Node MCU
/*-----( Importar Librerias )-----*/

#include <Arduino.h>
//#include <Wire.h>
#include <interfazSerial.h>
#include <InyecTime.h>
#include <variables.h>
#include <Sensores.h>
#include <CD4051.h>
#include <pines2.h>
#include <SPWM.h>
#include <AVC.h>


/*-----( Inicio de librerias )-----*/
AVC avc(dnt);
interfazSerial Ser(1);
SPWM pwm(byte(12), byte(45), pines, pinesE);

void setup() {
	//Wire.begin();
	Serial.println(F("DBG Iniciando"));
	attachInterrupt(digitalPinToInterrupt(2), I_RPM, CHANGE);
	Serial.print(F("DBG Tarde: "));
	Serial.print(millis());
	Serial.println(F("mS"));
	//analogWriteResolution(12);
}

void loop() {
	if (SINC) {
		pwm.Ecn(byte(12), byte(34)); //loop encendido
		pwm.Iny( avc.GetAVC(_RPM, _TEMP) ); //loop Inyeccion
		Ser.loop();
	}
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
	pwm.Intrr();
}


/*###############################################
########### Fixed Mode Loop & Control############
#################################################*/
//ARREGLAR PARA SOPORTAR NUEVO PROTOCOLO POR SERIE
//void FXM() {
//	//este seria seudo loop que tendria el modo fijo,
//
//	bool _FM = true;
//	String _t = "";
//	if (_msg && msg.startsWith("FXD.E")) {
//		do {
//			C_PWM();
//			HILO_1();
//			vent();
//
//			if (_msg) {
//				if (msg.startsWith("FXD.D")) { // "FX.D" Termina el modo fijo de inyeccion
//					_FM = false;
//				}
//				else if (msg.startsWith("INY")) { // "INY" cambia el tiempo de inyeccion
//					_t = msg.erase(0, 3);
//					INYT1 = _t.toInt();
//				}
//				else if (msg.startsWith("AVC")) { // "AVC" cambia el tiempo de avance
//					_t = msg.erase(0, 3);
//					AVC = _t.toInt();
//				}
//				//falta la parte de arranqued
//			}
//		} while (_FM);
//	}
//}
