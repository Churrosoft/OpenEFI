╔════════════════════════════════════════════════════════════════════════╗
║                    µEFI V 1.5X Team:                                   ║
║  Main Programer   FDSoftware | dfrenoux@gmail.com | fdsoftware.xyz     ║
║ Program features:                                                      ║
║    ∆ Control of diesel / petrol engines up to 4 cylinders              ║ 
║    ∆ Single pointinjection                                             ║
║    ∆ Independent ignition                                              ║
║    ∆ Error report                                                      ║ 
╚════════════════════════════════════════════════════════════════════════╝
/*-----( Importar Librerias )-----*/
#include <Arduino.h>

/*-----( Define's )-----*/
#define mtr 1   //definir tipo de motor 0 = diesel ; 1 = nafta
#define alpha 1 //Habilita el modo de prubas alpha
#define cil 4   //cantidad de cilindros del motor
/*-----( Variables Globales )-----*/
//estas variables las puede usar cualquieeer funcion
int _RPM       = 0,       //las rpm
    _TEMP      = 0,       //temperatura
    _POS       = 0,       //posicion del cigueñal (en dientes)
    _AE        = 0,       //avance de encendido
    dnt        = 150,     //numero de dientes del cigueñal
    _MAP       = 0,       //valor en Kpa de presion del multiple de admision
    _MAR       = 0,       //valor mariposa de admision
    _RLT       = 750;     //rpm minimas para Ralenti
bool DET       = false,   //variable para indicar si el motor esta detenido 
     ACL       = false,   //indica si se esta acelerando
     SINC      = false;   //indica si se sincronizo el PMS

/*-----( Variables C_PWM )-----*/

int AVC        = 15,      //Avance de encendido
    PMSI       = 30,      //Cantidad de dientes entre PMS
    AVCI       = 10,      //Avance de inyeccion
    PWM_FLAG_1 = 0,
    PWM_FLAG_1A= 0,
    PWM_FLAG_2 = 0,
    PWM_FLAG_3 = 0,
    INYT1      = 250,     //tiempo de inyeccion combustible
    INYT2      = 1800;    //tiempo de encendido bobina
  
/*-----( Variables RPM )-----*/

int  RPM_per   = 350;     //periodo en ms en el que se actualizan las rpm
long T_RPM_AC  = 0;       //para saber tiempo actual
long T_RPM_A   = 0;       //para saber tiempo anterior
int  _PR       = 0;       //numero de diente / pulso
unsigned long Tdnt[dnt]; //array con tiempo entre dientes
byte tdnt; //indice de Tdnt
/*-----( Variables INYECCION )-----*/
int INY[]     = {23,27,29,25};    //Pines de arduino que esta conectados los inyectores **CAMBIAR PINES**
int ECN[]     = {22,28,26,24};    //Pines del arduino que estan conectados a las bobinas(Encendido secuencial con 1 bobina por cilindro) **CAMBIAR PINES**
int mar       = A1,               //pin de mariposa de acelerador
    marv      = 0;                //valor actual de mariposa de acelerador
/*-----( TABLAS: )-----*/

int TINY[18][11]={
      //RPM requerida / carga motor
              //0 ;18;27;36;45;55;64;73;82;91;100 */
    /*800*/    {2 ,3 ,3 ,3 ,4 ,6 ,6 ,7 ,8 ,8 ,8 },
    /*1000*/   {2 ,3 ,3 ,4 ,5 ,8 ,8 ,8 ,10,10,10},
    /*1200*/   {2 ,5 ,5 ,5 ,6 ,9 ,9 ,10,11,12,12},
    /*1500*/   {7 ,8 ,8 ,8 ,8 ,10,10,11,13,14,14},
    /*1700*/   {9 ,10,10,10,10,11,11,12,14,15,15},
    /*2000*/   {11,12,12,12,12,13,13,14,15,16,16},
    /*2200*/   {12,14,14,14,14,15,15,16,17,18,18},
    /*2500*/   {14,16,16,16,16,17,17,17,18,19,19},
    /*2700*/   {16,18,18,18,18,19,19,19,20,21,21},
    /*3000*/   {18,20,20,20,20,21,21,21,22,23,23},
    /*3200*/   {20,22,22,22,22,23,23,23,24,25,25},
    /*3500*/   {22,24,24,24,24,25,25,25,26,27,27},
    /*3700*/   {24,26,26,26,26,27,27,27,28,29,29},
    /*4000*/   {25,27,27,27,27,28,28,28,29,30,30},
    /*4500*/   {27,28,28,29,29,30,30,30,31,32,32},
    /*5000*/   {29,31,32,32,32,33,33,33,34,35,35},
    /*5500*/   {31,33,33,33,33,33,33,34,36,37,37},
    /*6000*/   {33,34,34,34,34,34,34,35,37,38,38}
    };         //matriz tabla de inyeccion

 int tablaAvance[18][11]={//0 ;18;27;36;45;55;64;73;82;91;100 */
    /*800*/    {2 ,3 ,3 ,3 ,4 ,6 ,6 ,7 ,8 ,8 ,8 },
    /*1000*/   {2 ,3 ,3 ,4 ,5 ,8 ,8 ,8 ,10,10,10},
    /*1200*/   {2 ,5 ,5 ,5 ,6 ,9 ,9 ,10,11,12,12},
    /*1500*/   {7 ,8 ,8 ,8 ,8 ,10,10,11,13,14,14},
    /*1700*/   {9 ,10,10,10,10,11,11,12,14,15,15},
    /*2000*/   {11,12,12,12,12,13,13,14,15,16,16},
    /*2200*/   {12,14,14,14,14,15,15,16,17,18,18},
    /*2500*/   {14,16,16,16,16,17,17,17,18,19,19},
    /*2700*/   {16,18,18,18,18,19,19,19,20,21,21},
    /*3000*/   {18,20,20,20,20,21,21,21,22,23,23},
    /*3200*/   {20,22,22,22,22,23,23,23,24,25,25},
    /*3500*/   {22,24,24,24,24,25,25,25,26,27,27},
    /*3700*/   {24,26,26,26,26,27,27,27,28,29,29},
    /*4000*/   {25,27,27,27,27,28,28,28,29,30,30},
    /*4500*/   {27,28,28,29,29,30,30,30,31,32,32},
    /*5000*/   {29,31,32,32,32,33,33,33,34,35,35},
    /*5500*/   {31,33,33,33,33,33,33,34,36,37,37},
    /*6000*/   {33,34,34,34,34,34,34,35,37,38,38}
    };         //matriz tabla de avance

/*-----( Variables C_INY )-----*/
int INY_L = 150,   //tiempo de apertura del inyector en microsegundos
    INY_P = 500;   //tiempo en uS adicional para acelerar el motor

/*-----( Variables Ciclos )-----*/
int i    = 0,     //se usa para for
    i2   = 0;     //se usa para for anidado 1° nivel [ for(i){ for(i2){ } } ]

void setup(){
     for(i = 0; i < 4; i++){
       pinMode(INY[i], OUTPUT);
       pinMode(ECN[i], OUTPUT);
    }
    pinMode(A4, INPUT);
    pinMode(A8, INPUT);
    pinMode(6, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(2), I_RPM, CHANGE);
}

void loop(){
    if(SINC){ //solo cuando este sincronizado el PMS
        C_INY();  //control de tiempo inyeccion
        HILO_1(); //Control sensores
        C_PWM();  //control señal pwm
        _ACL();   //control de aceleracion
    }
}

void HILO_1(){
  //RPM / Temp / Sensores
  T_RPM_AC = millis();
  if (T_RPM_AC - T_RPM_A >= RPM_per) {
      T_RPM_A = T_RPM_AC;
      int _X_RPM = (_PR / dnt)*90; //calculo para obtener las rpm
      _PR = 0;
    }
      
}

int temp(){
  //esta funcion mide la temperatura y devuelve float en celsius
  //**solo para sensor LM35**
  #if alpha == 0
    int value = analogRead(sensorT);
    float millivolts = (value / 1023.0) * 5000;
    float celsius = millivolts / 10; 
    return celsius;
  #endif

  #if alpha == 1
     int val = analogRead(A4);
     return map(val, 0, 1023, 0 , 120);
  #endif
}

int _vmar(){
  //funcion para obtener la posicion de la mariposa de aceleracion
  #if alpha == 1
    int val = analogRead(A8);
    return map(val, 0, 1023, 0 , 99);
  #endif
}

void I_RPM(){ //interrupcion para rpm
    _PR++;
    _POS++;
    if(tdnt == 0){ Tdnt[tdnt] = micros(); tdnt++;}// para guardar primer temporizado del sincronizado
    Tdnt[tdnt] = micros() - Tdnt[(tdnt-1)];
    if(_POS >= dnt){
        if(!SINC){ sincronizar();}
      _POS = 0;
    }
    PWM_FLAG_1++;
    PWM_FLAG_1A++;
}

void _ACL(){
    //subrutina, hasta que no acelera no sale de aca

}
void C_INY(){
    //funcion que maneja el control de la inyeccion del motor
    if(_TEMP >= 46 && !ACL){
        INYT1 = Tiny(_RPM, _MAR,1);
    }
    if(_RPM < _RLT && !ACL){
        INYT1 = Tiny(_RPM, _MAR,3);
    }
}

int Tiny(int rpm2, int marv2, int OP){
    //con esta funcion se obtiene el tiempo base de inyeccion y luego se puede
    //recalcular dependiendo de la necesidad
	int rpm3  = map(rpm2,0,7000,0,17); //aproximamos las rpm
	int marv3 = map(marv2,0,255,0,10); //aproximamos el valor de la mariposa de aceleracion
    return TINY[marv3,rpm3];
}


void C_PWM(){
 bool C1 = (PWM_FLAG_1 >= (PMSI - AVCI)); //Condicional para inyeccion
 #if mtr == 1
 bool C2 = (PWM_FLAG_1A >= (PMSI - AVC));  //Condicional para encendido
 #endif
//*_*_*_*_*_*CONTROL INYECCION*_*_*_*_*_*_*
  if(C1){
     digitalWrite(INY[PWM_FLAG_2],!digitalRead(INY[PWM_FLAG_2]));
     delayMicroseconds(INYT1);
     digitalWrite(INY[PWM_FLAG_2],!digitalRead(INY[PWM_FLAG_2]));
     PWM_FLAG_2++;
     PWM_FLAG_1 = 0;
  }
  if(PWM_FLAG_2 > (cil - 1)){
     PWM_FLAG_2 = 0;
  }
//*_*_*_*CONTROL DE ENCENDIDO *_*_*_*_*_*_*_*_*_*_*
#if mtr == 1
  if(C2){
    digitalWrite(ECN[PWM_FLAG_3],!digitalRead(ECN[PWM_FLAG_3]));
    delayMicroseconds(INYT2);
    digitalWrite(ECN[PWM_FLAG_3],!digitalRead(ECN[PWM_FLAG_3]));
    PWM_FL AG_3++;
    PWM_FLAG_1A = 0;
  }
  if(PWM_FLAG_3 > (cil - 1)){
    PWM_FLAG_3 = 0;
  }
#endif
}

#if (mtr == 1)
int AVC_E(){
    if(_RPM < 600){
    //controlador de avance en frio
        return avanceArranque();
    }else if(_RPM >=600){
    return avanceNormal();
    }
  }

int avanceArranque(){
  int avc2 = dientes(3);
  return avc2;
}

int avanceNormal(){
  int avc2 = tablaAvance[map(_RPM,0,8000,0,17)][map(temp(),0,100,0,10)];
  return dientes(avc2);//retornamos el avance en cantidad de dientes
}
#endif

//convierte grados en dientes del sensor hall
int dientes(float grados){
  float grad = 360 / dnt; //dividimos 360 grados por la cantidad de dientes
  //dividimos grados por grad, luego multiplicamos por 100 para transformar el float en int
  int x2     = (grados / grad) * 100; 
  //dividimos por 100, al hacer esto se eliminan los decimales, en prox ver redondear
  int dnt2    = x2 / 100;
  return dnt2; 
}

void sincronizar(){
        //Esta funcion sincroniza el valor de "vuelta" con el PMS del piston 1
        int TMIN = 99999; //almacena el tiempo menor de cada diente
        int IMIN = 0; //almacena el indice del menor tiempo(al final no la tuve que usar :P)
        int TMAX = 0; //almacena el tiempo mayor (Este seria teoricamente el PMS del piston 1)
        int IMAX = 0; //almacena el indice del diente con PMS del piston 1, luego se restablece la variable diente...
        //a 0 y se pone la variable sincronizado en true para habilitar el resto del programa.
    
        int DientesEntreHUECOyPMS = dnt;//esta variable la cambiamos cuando sepamos cuantos dientes son
        int TamanoDelArray = (sizeof(Tdnt)/sizeof(Tdnt[0]));//Tama�o del array (cantidad de dientes)
    
        for(int i = 0; i < TamanoDelArray; i++){
                if(Tdnt[i] < TMIN){
                    TMIN = Tdnt[i];
                }else if(TMAX < Tdnt[i]){
                    TMAX = Tdnt[i];
    
                    IMAX = i + DientesEntreHuecoYPMS;//PMS del piston uno seria...
                    //el indice del hueco(tiempo mayor) mas la distancia entre el hueco...
                    //y el pms del piston uno
                }
            }
        diente = TamanoDelArray-IMAX;//el diente actual es la distancia que tenemos..
        //desde el diente 0 que es el diente del pms del piston 1(IMAX)
    
    
        //for para obtener el promedio de tiempo entre dientes y modificarlo en la clase variables.h
        int aux = 0;
        for(int i = ; i < TamanoDelArray; i++){
            aux+=Tdnt[i];
        }
        promedio = aux/TamanoDelArray;//fianlmente cambiamos el promedio
        //una vez ejecutada esta funcion se puede usar con seguridad la variable dientes y el promedio :P
        vuelta = 0;
        SINC = true;
}