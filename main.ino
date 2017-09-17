/***
 *     _____                       _____ ______  _____         _   _    __      _____ __   __
 *    |  _  |                     |  ___||  ___||_   _|       | | | |  /  |    |  ___|\ \ / /
 *    | | | | _ __    ___  _ __   | |__  | |_     | |         | | | |  `| |    |___ \  \ V / 
 *    | | | || '_ \  / _ \| '_ \  |  __| |  _|    | |         | | | |   | |        \ \ /   \ 
 *    \ \_/ /| |_) ||  __/| | | | | |___ | |     _| |_        \ \_/ /  _| |_ _ /\__/ // /^\ \
 *     \___/ | .__/  \___||_| |_| \____/ \_|     \___/         \___/   \___/(_)\____/ \/   \/
 *           | |                                                                             
 *           |_|                                                                             
 */
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
╚════════════════════════════════════════════════════════════════════════╝
*/
/*-----( Importar Librerias )-----*/
#include <Arduino.h>

/*-----( Declarar variables :D )-----*/

/*-----( Define's )-----*/

#define mtr 1   //definir tipo de motor 0 = diesel ; 1 = nafta
#define dev 1   //habilita modo desarollo
#define cil 4   //cantidad de cilindros del motor

/*-----( Variables Globales )-----*/
//estas variables las puede usar cualquieeer funcion

int _RPM       = 0, 			//las rpm :V
	_TEMP      = 0, 			//temperatura
	_POS       = 0, 			//posicion del cigueñal (en dientes)
	_AE        = 0, 			//avance de encendido
	dnt        = 120; 			//numero de dientes del cigueñal

/*-----( Variables CPWM )-----*/

int AVC        = 12,			//Avance de encendido
    PMSI       = 45,			//Cantidad de dientes entre PMS
    AVCI       = 2,				//Avance de inyeccion
    PWM_FLAG_1 = 0,
    PWM_FLAG_1A= 0,
    PWM_FLAG_2 = 0,
    PWM_FLAG_3 = 0,
    INYT1      = 0,       //tiempo de inyeccion combustible
    INYT2      = 0;       //tiempo de encendido bobina
  
/*-----( Variables RPM )-----*/

int  RPM_per   = 500;		  //periodo en ms en el que se actualizan las rpm
long T_RPM_AC  = 0;   		  //para saber tiempo actual
long T_RPM_A   = 0;  		  //para saber tiempo anterior
int  _PR       = 0;  		  //numero de diente / pulso

/*-----( Variables INYECCION )-----*/
int INY[]     = {3,4,5,6};    //Pines de arduino que esta conectados los inyectores **CAMBIAR PINES**
int mar       = A1,           //pin de mariposa de acelerador
	marv      = 0;            //valor actual de mariposa de acelerador

/*-----( Variables Comunicacion )-----*/
bool _msg = false;          //si esta en true hay un mensaje nuevo del puerto serie
String msg = "";            //Mensaje que llego por serie

/*-----( Variables Ciclos )-----*/
int i		 = 0,			//se usa para for
	i2		 = 0;			//se usa para for anidado 1° nivel [for(i){for(i2){}}]
void setup(){
     Serial.begin(9600);
   //Iniciamos "hilos"
    Serial.println("DBG : ARANCAMO WASHOOOO");
	bool _CPIN = (i == );//Condicional para definir pines salida
	 for(i = 0, i){
	 }
}

void loop(){
    //este hilo se encarga de la comunicacion con pc y programacion
    if(_msg){ //Nos llego un mensaje desde la pc y nos ponemo re felices :3 XDD
    Serial.print("DBG Mensaje:");
    Serial.println(msg);
    msg  = ""; //vaciamo la variable apra que no rompa las bola :3
        _msg = false;
    }
    HILO_1();
    I_RPM();
    C_PWM();
}

//funcion para manejo de enventos por serie
void serialEvent(){
    while(Serial.available()){
        char _T = (char)Serial.read();
        msg += _T;
        if (_T == '\n') {
            _msg = true;
        }
    }
}

void HILO_1(){
  //RPM / Temp / Sensores
  T_RPM_AC = millis();
  if (T_RPM_AC - T_RPM_A >= RPM_per) {
         T_RPM_A = T_RPM_AC;
         int _X_RPM = (_PR / dnt)*90; //calculo para obtener las rpm
         Serial.print("RPM ");
         Serial.println(_X_RPM);
         _PR = 0;
    }
}
void I_RPM(){ //interrupcion para rpm
    _PR++;
    _POS++;
    if(_POS >= dnt){
      _POS = 0;
    }
    PWM_FLAG_1++;
    PWM_FLAG_1A++;
    delayMicroseconds(125);
}

void C_PWM(){
  bool C1 = (PWM_FLAG_1 >= (PMSI - AVCI)); //Condicional para inyeccion
 #if mtr == 1
 bool C2 = (PWM_FLAG_1A >= (PMSI - AVC));  //Condicional para encendido
 #endif
//*_*_*_*_*_*CONTROL INYECCION*_*_*_*_*_*_*
  if(C1){
    V[FLAG_2] = digitalRead(INY[FLAG_2]);
		digitalWrite(INY[FLAG_2],!V[FLAG_2]);
		delayMicroseconds(INYT1);
		digitalWrite(INY[FLAG_2],V[FLAG_2]);
		 PWM_FLAG_2++;
		 PWM_FLAG_1 = 0;
  }
  if(PWM_FLAG_2 > (cil - 1)){
    PWM_FLAG_2 = 0;
  }
//*_*_*_*CONTROL DE ENCENDIDO *_*_*_*_*_*_*_*_*_*_*
#if mtr == 1
  if(C2){
    V[FLAG_3] = digitalRead(ECN[FLAG_3]);
		digitalWrite(ECN[FLAG_3],!V[FLAG_3]);
		delay(INYT2);
		digitalWrite(ECN[FLAG_3],V[FLAG_3]);
    PWM_FLAG_3++;
    PWM_FLAG_1A = 0;
  }
  if(PWM_FLAG_3 > (cil - 1)){
    PWM_FLAG_3 = 0;
  }
#endif
}