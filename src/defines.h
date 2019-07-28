// Acá todos los defines

/*-----(Globales )-----*/

#define CIL 4       //cantidad de cilindros o pistones, o camaras de combustion, etc ?)
#define DNT 60      //cantidad de dientes del sensor CKP
#define Alpha 1     //modo para pobrar sin correcciones de tiempo, ni algoritmos de inyeccion ni sincronizacion, para encajar un 555 y probar a pelo ?)

/*-----( RPM )-----*/

#define RPM_per 500 //periodo en ms en el que se actualizan las rpm ( si lo cambias , o arreglas el calculo para las rpm,o se rompe todo maquinola)

/*-----( C_PWM )-----*/

#define ECNT 250 //tiempo en ms que se prende la bobina (cambie a su riesgo)
#define PMSI 30  //Cantidad de dientes entre PMS
#define AVCI 10  //avance de inyeccion (si queres quemar las valvulas dejalo en 0)