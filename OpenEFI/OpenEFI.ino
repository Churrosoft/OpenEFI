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
#include "Relay.h"
#define test 1  //modo pruebas con placa debug
#define mtr 1   //definir tipo de motor 0 = diesel ; 1 = nafta
#define dev 1   //habilita modo desarollo
#define alpha 1 //Habilita el modo de prubas alpha
#define GNC_MOD 0 //activa el modo de GNC/GLP en la inyeccion
#define uEFI 0 //en 1 si se compila para una placa uEFI
#define OpenEFImode 0 // en 0 si es OpenEFI con Arduino nano, en 1 si utiliza un Node MCU

/*-----( Importar Librerias )-----*/
#include <Arduino.h>
#include <Wire.h>
#include "interfazSerial.h"
#include "InyecTime.h"
#include "variables.h"
#include "Sensores.h"
#include "Debug.h"
#include "CD4051.h"
#include "pines2.h"
#include "SPWM.h"
#include "AVC.h"
#include "Memory.h"
#include "DTC.h"
#include "Relay.h"

/*-----( Inicio de librerias )-----*/
Memory m(0);
Sensores s_main(0);
DTC dtcmain(m,s_main);
AVC avc(dnt, s_main, m);
InyecTime time(s_main,m);
interfazSerial Ser(1);
Debug dbg(Ser,m);
SPWM pwm(byte(12), byte(45), pines, pinesE);


//por ahora las unicas tablas que sincronizan con la memoria son las de avance e inyeccion
//BUG: abrir todas las tablas a la vez satura la RAM
//NO SIRVE, MUCHA RAM AL SOPE, PASAR OBJETO DE MEMORIA POR REFERENCIA Y LISTO
//edit, se podria cachear los tiempos cercanos para mejorar la reaccion
//int **Avance  = calloc(11, 18); //tabla de avance
//int **InyTime = calloc(11, 18); //tabla de tiempo de inyeccion
//int **VE      = calloc(11, 18);
bool _fxm = false;

void setup() {
	Serial.println(F("DBG Iniciando"));
	time.TLamb(1500, 45);
	time.Tarr();

	Wire.begin();
	attachInterrupt(digitalPinToInterrupt(2), I_RPM, CHANGE);
	Serial.print(F("DBG Tarde: "));
	Serial.print(millis());
	Serial.println(F("mS"));
}

void loop() {
#if test == false
	dtcmain.DTC_Loop();
	if (SINC) {
#if alpha == 1
		pwm.Iny(time.Aphplus(_RPM));
#endif
#if alpha == 0
		pwm.Iny(byte(12)); //loop Inyeccion
#endif
		pwm.Ecn(avc.GetTime(), avc.GetAVC(_RPM)); //loop encendido
		vent(); //control de electro ventilador
		FXM(); //FixedMode
		dbg.loop(_RPM); //Debug
	}
#endif
#if test == true
	//el tiempo de inyeccion depende del potenciometro de la mariposa, solo para test de placas
	pwm.Set( map( analogRead(A1) , 0, 2500, 0, 1024) );
	pwm.Iny();
#endif // test == true

}

void Iny() {
	//UNDONE: void Iny()
	//este void elige que algoritomo usar par calcular el tiempo de inyeccion
	//tambien se puede anular para usar el algoritmo que el usuario quiera
	/* basicamene:
		si esta frio uso tabla + un plus para calentar rapido
		en caliente se usa Aphplus en aceleracion (agregar un plus por las dudas luego)
		en ralenti se usa BPW con lambda*/

	if ( s_main.Temp() < 45 )
		//la wea esta fria:
		pwm.Set( time.TTable(_RPM) + 120 ); //120 uS de "plus" , luego modificar con #define
	if ( s_main.Temp() > 45 )
		pwm.Set( time.Aphplus(_RPM) );
	if ( s_main.Temp() > 45 && _RPM < 1200 )
		pwm.Set( time.BPW() );
	if ( s_main.Temp() >= 105 && _RPM < 1200) //sobre temperatura
		pwm.Set(0);
}

void I_RPM() { //interrupcion para rpm
	if (SINC) {
		_POS++;
		_POS2++;
		pwm.Intrr();
	}
	if (!SINC)
		sincINT();
}

void vent() {
	//Controla electroventilador y corte de inyeccion por sobretemperatura
	//if (main.Temp() > 75)
		
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
/*###############################################
########### Fixed Mode Loop & Control############
#################################################*/
void FXM() {
//este seria seudo loop que tendria el modo fijo,
	if (Ser.FXMD())
		_fxm = true;
		int iny2, avc2;
		do {
			vent();
			iny2 = Ser.FXMD(true , iny2);
			avc2 = Ser.FXMD(false, avc2);
			pwm.Set(iny2);
			pwm.Iny(); //loop Inyeccion
			pwm.Ecn(avc.GetTime(), avc2); //loop encendido
		} while (Ser.FXMD());
}
