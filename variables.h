#pragma once
/*-----( Variables Globales )-----*/
//estas variables las puede usar cualquieeer funcion

int _RPM = 0,       //las rpm
_TEMP = 0,       //temperatura
_POS = 0,       //posicion del cigueñal (en dientes)
_AE = 0,       //avance de encendido
dnt = 150,     //numero de dientes del cigueñal
_MAP = 0,       //valor en Kpa de presion del multiple de admision
_MAR = 0,       //valor mariposa de admision
_RLT = 750;     //rpm minimas para Ralenti
bool DET = false,   //variable para indicar si el motor esta detenido 
ACL = false,   //indica si se esta acelerando
SINC = false;   //indica si se sincronizo el PMS

/*-----( Variables C_PWM )-----*/

int AVC = 15,   //Avance de encendido
PMSI = 30,      //Cantidad de dientes entre PMS
AVCI = 10,      //Avance de inyeccion
PWM_FLAG_1 = 0,
PWM_FLAG_1A = 0,
PWM_FLAG_2 = 0,
PWM_FLAG_3 = 0,
INYT1 = 250,     //tiempo de inyeccion combustible
INYT2 = 1800;    //tiempo de encendido bobina

/*-----( Variables RPM )-----*/

int  RPM_per = 350;     //periodo en ms en el que se actualizan las rpm
long T_RPM_AC = 0;       //para saber tiempo actual
long T_RPM_A = 0;       //para saber tiempo anterior
int  _PR = 0;       //numero de diente / pulso
unsigned long Tdnt[150]; //array con tiempo entre dientes
byte tdnt; //indice de Tdnt

/*-----( Variables INYECCION )-----*/
int INY[] = { 23,27,29,25 };    //Pines de arduino que esta conectados los inyectores **CAMBIAR PINES**
int ECN[] = { 22,28,26,24 };    //Pines del arduino que estan conectados a las bobinas(Encendido secuencial con 1 bobina por cilindro) **CAMBIAR PINES**
int marv = 0;                //valor actual de mariposa de acelerador

/*-----( Variables C_INY )-----*/
int INY_L = 150,   //tiempo de apertura del inyector en microsegundos
INY_P = 500,   //tiempo en uS adicional para acelerar el motor
INY_C = 25000; //Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiométrica (lambda=1 ) , para cilindrada del motor , presión a 100kPa , temperatura del aire a 21ºC y VE 100% .
byte AF = 147;   //mexcla Aire Combustible objetivo numero entero sin coma, para sacar la coma se multiplica por 10, asi que para tener una mexcla de 14,7/1 se escribe 147
byte TBM = 100;     //TBM - Turbo Boost Multiplier (valor multiplicado por 100) boost de 1,2 = 120
					/*-----( Variables _LMB )-----*/
bool LMBM = false; //en true si se utilizan las dos sondas lambda
int
CTA = 250, //Correcion de tiempo A, mezcla rica, se le sacan X uS
CTB = 300, //Correcion de tiempo B, mezcla pobre, se le agregan X uS
P_LMB = 250, //periodo en mS en el que se corrije por sonda lamba
T_LMB = 45;  //temperatura a partir de la cual se intenta correjir el tiempo de inyeccion

float FLMBA = 1.5,  //factor maximo de lambda
FLMBB = 0.85; //factor minimo de lambda

unsigned long
T_LMB_AC = 0,  //para saber tiempo actual  (Temporizador correccion por sonda lambda)
T_LMB_A = 0;  //para saber tiempo anterior                     (||)

/*-----( Variables Comunicacion )-----*/
bool _msg = false;           //si esta en true hay un mensaje nuevo del puerto serie
String msg = "";              //Mensaje que llego por serie

/*-----( Variables Ventilador )-----*/
byte vmt = 70; //temperatura en °C que se prende el ventilador
byte vmt2 = 55; //temperatura en °C que se apaga el ventilador


/*-----( Variables Ciclos )-----*/
int i = 0,     //se usa para for
i2 = 0;     //se usa para for anidado 1° nivel [ for(i){ for(i2){ } } ]