// Acá todos los defines

/*-----( backend turbina: )-----*/
#define USART1_BAUD 230400 //Baud de USART1 , 8 bits de data y uno de stop, no me vengan con cosas mas raras que eso
/*-----(Globales )-----*/

#define mtr 1       //habilita encendido
#define CIL 4       //cantidad de cilindros o pistones, o camaras de combustion, etc ?)
#define DNT 60      //cantidad de dientes del sensor CKP
#define Alpha 1     //modo para pobrar sin correcciones de tiempo, ni algoritmos de inyeccion ni sincronizacion, para encajar un 555 y probar a pelo ?)

/*-----( RPM )-----*/

#define RPM_per 500 //periodo en ms en el que se actualizan las rpm ( si lo cambias , o arreglas el calculo para las rpm,o se rompe todo maquinola)

/*-----( C_PWM )-----*/

#define ECNT 250 //tiempo en ms que se prende la bobina (cambie a su riesgo)
#define PMSI 30  //Cantidad de dientes entre PMS
#define AVCI 10  //avance de inyeccion (si queres quemar las valvulas dejalo en 0)

/*-----( I_IALG )-----*/
//cuando tenga la libreria de memoria paso todo a variable, por ahora con define
//int INY_L = 150,   //tiempo de apertura del inyector en microsegundos
//INY_P = 500,   //tiempo en uS adicional para acelerar el motor
//INY_C = 25000; //Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiom�trica (lambda=1 ) , para cilindrada del motor , presi�n a 100kPa , temperatura del aire a 21�C y VE 100% .

#define INY_C 8000
#define INY_P 500
#define INY_L 150
#define ARRTp 45 //temperatura para definir si es arranque en frio o en caliente
#define TARR  3500 //tiempo de inyeccion para arranque (frio)
#define TARR2 2500 //tiempo de inyeccion para arranque (caliente)
/*-----( Variables _LMB )-----*/

#define LMBM  false //en true si se utilizan las dos sondas lambda
#define CTA   250 //Correcion de tiempo A, mezcla rica, se le sacan X uS
#define CTB   300 //Correcion de tiempo B, mezcla pobre, se le agregan X uS
#define P_LMB 250 //periodo en mS en el que se corrije por sonda lamba
#define T_LMB 45 //temperatura a partir de la cual se intenta correjir el tiempo de inyeccion
#define FLMBA 1.5 //factor maximo de lambda
#define FLMBB 0.85 //factor minimo de lambda

/*-----( Variables BPW )-----*/

#define BPC 1500  //Base Pulse Constant
#define AF 123 // relacion aire combustible, se divide por 10 para no usar float
#define BVC 1 //correcion por bateria, luego intentar usar tabla
#define BLM 168 // "Block Learn" varia entre 168 y 170 dependiendo de como resultaron los tiempos
//anteriores, seudo IA para mejorar el tiempo de inyeccion
#define DFCO 1 //ni puta idea
#define DE 1 // ni puta idea x2
#define TBM 1 //turbo boost multiplier DEJAR EN 1 CARAJO, que sino rompes todo