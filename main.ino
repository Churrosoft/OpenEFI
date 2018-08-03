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
#include <Arduino.h>
#include <malloc.h>
#include <Wire.h>

/*-----( Define's )-----*/

#define mtr 1   //definir tipo de motor 0 = diesel ; 1 = nafta
#define dev 1   //habilita modo desarollo
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

int VE[18][11]={
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
    };         //matriz tabla de Eficiencia Volumentrica

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
    INY_P = 500,   //tiempo en uS adicional para acelerar el motor
    INY_C = 25000; //Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiométrica (lambda=1 ) , para cilindrada del motor , presión a 100kPa , temperatura del aire a 21ºC y VE 100% .

/*-----( Variables _LMB )-----*/
bool LMBM = false; //en true si se utilizan las dos sondas lambda
int
    LMBA  = A4,  //pin sonda lambda A
    LMBB  = A5,  //pin sonda lambda B
    CTA   = 250, //Correcion de tiempo A, mezcla rica, se le sacan X uS
    CTB   = 300; //Correcion de tiempo B, mezcla pobre, se le agregan X uS
    P_LMB = 250; //periodo en mS en el que se corrije por sonda lamba
    T_LMB = 45;  //temperatura a partir de la cual se intenta correjir el tiempo de inyeccion

float FLMBA = 1.5,  //factor maximo de lambda
      FLMBB = 0.85; //factor minimo de lambda

unsigned long 
     T_LMB_AC  = 0,  //para saber tiempo actual  (Temporizador correccion por sonda lambda)
     T_LMB_A   = 0;  //para saber tiempo anterior                     (||)

/*-----( Variables Comunicacion )-----*/
bool _msg     = false;           //si esta en true hay un mensaje nuevo del puerto serie
String msg    = "";              //Mensaje que llego por serie


/*-----( Variables Ventilador )-----*/
// VM = modo del ventialdor, 1 = fijo, a partir de x temperatura se prende, 0 = control de velocidad variable por pwm
#define VM 1
byte vmt  = 70; //temperatura en °C que se prende el ventilador
byte vmt2 = 55; //temperatura en °C que se apaga el ventilador
byte vmtP = 4;  //pin de ventialador

/*-----( Variables Ciclos )-----*/
int i    = 0,     //se usa para for
    i2   = 0;     //se usa para for anidado 1° nivel [ for(i){ for(i2){ } } ]

/*-----( Variables Free RAM )-----*/
int  RAM_per   = 1200;     //periodo en ms en el que se actualiza la ram libre
long T_RAM_AC  = 0;        //para saber tiempo actual
long T_RAM_A   = 0;       //para saber tiempo anterior
extern char _end;
extern "C" char *sbrk(int i);
char *ramstart=(char *)0x20070000;
char *ramend=(char *)0x20088000;

void setup(){
    Wire.begin();
     Serial.begin(115200);
     Serial.println("DBG Iniciando");
     for(i = 0; i < 4; i++){
       pinMode(INY[i], OUTPUT);
       pinMode(ECN[i], OUTPUT);
    }
    pinMode(A4, INPUT);
    pinMode(A8, INPUT);
    pinMode(6, OUTPUT);
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
    analogWriteResolution(12);
}

void loop(){
    if(SINC){ //solo cuando este sincronizado el PMS
        C_INY();  //control de tiempo inyeccion
        HILO_1(); //Control sensores
        C_PWM();  //control señal pwm
        vent();   //control ventialdor
        _ACL();   //control de aceleracion
        #if dev == 1
            FXM();  //FixedMode
        #endif
    }
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
      Serial.print("AVC ");
      Serial.println(AVC_E());
    }

    T_RAM_AC = millis();
    if (T_RAM_AC - T_RAM_A >= RAM_per) {
      //Actualizamos la memoria libre
      T_RAM_A = T_RAM_AC;
      char *heapend=sbrk(0);
      register char * stack_ptr asm ("sp");
      struct mallinfo mi=mallinfo();
      Serial.print("RAM  ");
      float RAM = (stack_ptr - heapend + mi.fordblks) * 0.001 ;
      Serial.println(RAM);
      delay(5);
      Serial.print("TMP ");
      Serial.println(temp());
      delay(5);
      Serial.print("LAD ");
      Serial.println(_vmar());
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


void vent(){
  //funcion que controla el ventilador
  #if VM == 0
    analogWrite(6, map(temp(),0,120,0,254));
  #endif
  #if VM == 1
    if(_TEMP > vmt){
        digitalWrite(vmtP, HIGH);
    }
    if(_TEMP > vmt2){
        digitalWrite(vmtP, LOW);
    }
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
	switch(OP){ 
        case 0://Tiempo de inyeccion base
            return TINY[marv3,rpm3];
         break;

        case 1://Tiempo de inyeccion base + correccion por lambda
            return _LMB(TINY[marv3,rpm3]);
         break;
         case 2: //ALPHA-N con MAP
            return _APHPLUS();
         break;
         case 3: //Tiempo de inyeccion para arrancar
            return INY_L + (INY_P *2);
         break;
    }
}

int _Ctemp(int tin){
    //esta funcion corrige tiempo de inyeccion dependiendo de la temperatura y rpm

}
int _APHPLUS(){ //ALPHA-N con MAP
/*
 PW = [INJ_CONST * VE(tps,rpm) * MAP * AirDensity] + AccEnrich +InjOpeningTime

 PW (pulse width) ---el tiempo final de apertura del inyector .

 INJ_CONST ----Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiométrica (lambda=1 ) , para cilindrada del motor , presión a 100kPa , temperatura del aire a 21ºC y VE 100% .

 VE para Speed Density (map, rpm)----Valor de eficiencia volumétrica tomada de la mapa VE .

 VE para ALPHA-N (tps, rpm)----Valor de eficiencia volumétrica tomada de la mapa VE .

 MAP(manifold absolute pressure)--- Presión tomada en el colector de admisión .

 AirDensity ---- Porcentual diferencia de densidad de aire comparada con la densidad de aire a temperatura 21ºC

 AccEnrich (acceleration enrichment) ---- Enriquecimiento de la mezcla en fase de aceleración .

 InjOpeningTime ---- Tiempo de apertura de inyector hasta el momento de inicio de inyección de combustible ( valor de retardo tomado de la mapa de calibración , INJECTORS CAL. )
*/
 return (INY_C * (VE[map(_MAP,0,150,0,10), _rpm] /100 ) * _MAP * 1.20) + INY_P + INY_L ;
}
int _LMB(int T){

    //Esta funcion mide la/s sonda lambda y corrige la mezcla / 
    //tiempo de inyeccion
    float _LMB_XA = analogRead(LMBA) * (3.3 / 1023); //medimos la sonda A
    float _LMB_XB = 0;
    //V menor a 0.45 = mezcla pobre
    if(_LMB_XA < 0.45){ //aca comparo por Volt pero tendriaaaa que hacerlo con los el factor lamba :S
        T = T + CTB;
    } else if(_LMB_XA > 0.45){ //V mayor a 0.45 = mezcla rica
        T = T - CTA;
    }
    if(LMBM){ 
        _LMB_XB = analogRead(LMBB) * (3.3 / 1023); //medimos la sonda B (Si corresponde)
        //V menor a 0.45 = mezcla pobre
        if(_LMB_XB < 0.45){ //aca comparo por Volt pero tendriaaaa que hacerlo con los el factor lamba :S
             T = T + CTB;
        } else if(_LMB_XB > 0.45){ //V mayor a 0.45 = mezcla rica
            T = T - CTA;
        }
    }
    return T;
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

int AVC_E(){ //*****CAMBIAR*******
    //----CONTROL DE AVANCE ANTES DE MANDAR CHISPA----
    if(_RPM < 600){
    //controlador de avance en frio
        return avanceArranque();
    }else if(_RPM >=600){
    return avanceNormal();
    }
    //---------------FIN DE CONTROL DE AVANCE PARA PROCEDER A MANDAR CHISPA-----------
  }
//--------------------BLOQUE DE CONTROLADORES DE AVANCE--------------------------

int avanceArranque(){
  int avc2 = dientes(3);
  return avc2;
}

int avanceNormal(){
  int temperatura = temp();
  int indiceTemp = 0;//indice que indica a que columna de la matriz acceder(temperatura)
  int indiceRPM = 0;//indice que indica a que fila de la matriz acceder(RPM)

    if(temperatura < 98){
    indiceTemp = ((temperatura < 18)? 0:((temperatura < 27)? 1:((temperatura < 36)? 2:((temperatura < 45)? 3:((temperatura < 55)? 4:((temperatura < 64)? 5:((temperatura < 73)? 6:((temperatura < 82)? 7:((temperatura < 91)? 8:9)))))))));
    //si temperatura < 18 leer columna 0 en tabla, si temperatura < 27 leer columna 1, 
    //si temperatura < 36 leer columna 2 etc etc.
  }else{
    indiceTemp = 10;
    //si la temperatura es mayor a 98 leemos la ultima columna en tabla(la mayor)
  }
  indiceRPM = ((_RPM<1000)?0:((_RPM<1200)?1:((_RPM<1500)?2:((_RPM<1700)?3:((_RPM<2000)?4:((_RPM<2200)?5:((_RPM<2500)?6:((_RPM<2700)?7:((_RPM<3000)?8:((_RPM<3200)?9:((_RPM<3500)?10:((_RPM<3700)?11:((_RPM<4000)?12:((_RPM<4500)?13:((_RPM<5000)?14:((_RPM<5500)?15:((_RPM<6000)?16:17))))))))))))))))); 
  //si rpm < 1000, leer columna 0 en tabla... si rpm < 1200 leer fila 1, si rpm < 1500 leer fila 2, etc etc.
  
  int avc2 = dientes(tablaAvance[indiceRPM][indiceTemp]);
  //finalmente accedemos al valor en tabla correspondiente al estado actual del motor

  return avc2;
  
}
//--------------------FIN DE BLOQUE DE CONTROLADORES DE AVANCE----------------

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

/*###############################################
########### Fixed Mode Loop & Control############  
#################################################*/

void FXM(){
    //este seria seudo loop que tendria el modo fijo,

    bool _FM = true;
    String _t = "";
    if(_msg && msg.startsWith("FXD.E")){
        do{
            C_PWM();
            HILO_1();
            vent();

            if(_msg){
                if(msg.startsWith("FXD.D")){ // "FX.D" Termina el modo fijo de inyeccion
                    _FM = false;
                }else if(msg.startsWith("INY")){ // "INY" cambia el tiempo de inyeccion
                    _t = msg.erase(0,3);
                    INYT1 = _t.toInt();
                }else if(msg.startsWith("AVC")){ // "AVC" cambia el tiempo de avance
                    _t = msg.erase(0,3);
                    AVC = _t.toInt();
                }
                //falta la parte de arranque
                _msg = false;
            }
        }while(_FM);
    }
}

void sincronizar(){
    
        #if(dev == 1) 
            if (sincronizado == false){
                Serial.println("DBG Sincronizando PMS con inyeccion y encendido");
            }
       #endif
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
    