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
#include <Scheduler.h>
#include <Adafruit_LiquidCrystal.h>
#include <Wire.h>

/*-----( Declarar variables :D )-----*/

#define motor 1 //definir tipo de motor 0 = diesel ; 1 = nafta
#define dev 1   //habilita mensajes por serial
String ver = "## ver 0.01 Alpha ##";
int cilindros = 4;   //cantidad de cilindros del motor

/*-----( Variable Globales )-----*/
//estas variables las puede usar cualquieeer funcion
int _RPM       = 0; //las rpm :V
int _TEMP      = 0; //temperatura
int _POS       = 0; //posicion del cigueñal (en dientes)
int _AE        = 0; //avance de encendido

/*-----( Variables RPM )-----*/
int  dnt	   = 20; //mumero de dientes del cigueñal
int  RPM_per   = 500;//periodo en ms en el que se actualizan las rpm
long T_RPM_A   = 0;	 //tiempo del periodo anterior de rpm
long T_RPM_AC  = 0;  //tiempo actual para rpm
int  _PR       = 0;  //numero de diente / pulso
long Tdnt[200];      // array con tiempo de pines de toda la vuelta del cigueñal
long Tant      = 0;  //variable con valor de micros() del diente anterior
int i2         = 0;  //variable para recorrer array Tdnt
bool sincronizado = false; //medio obvio true si esta sincronizado el motor

/*-----( Variables INYECCION )-----*/
int iny[]    = {3,4,5,6};    //Pines de arduino que esta conectados los inyectores **CAMBIAR PINES**
int inyT     = 0;            //tiempo de inyeccion   
bool arr     = false;        //si se intenta arrancar el motor esta en true

int mar       = A1;           //pin de mariposa de acelerador
int marv      = 0;            //valor actual de mariposa de acelerador
bool inye     = false;       //variable para saber si se esta inyectando todavia o no

bool acl;                   //varible temporar, guarda si se intento acelerar el motor
bool per;

int vuelta2  = 0;           //numero de vuelta para inyeccion
int dntact   = 0;			//diente actual del cigueñal
int dntPER   = 89;		    //cantidad de dientes entre PMS
int dntIAVC  = 12;          //dientes de avance entre PMS

int ACL_PER  = 15;			//cantidad de vueltas que ACL espera para cambiar el tiempo de inyeccion
int ACL_TMP  = 1500;		//cantidad de tiempo de inyeccion que ACL sube o baja

int ARR_TINY1= 25000; 		//tiempo de inyeccion en arranque frio
int ARR_PER  = 5; 			//primer periodo de vueltas, en este se espera que pase este valor con el tiempo base de arranque
int ARR_RPM  = 0;			//rpm para arranque

int ARR_FLAG2 = 800;		//cantidad de vueltas maximas para considerar que se intenta arrancar el motor
int ARR_FLAG1= 0;			//FLAG para control de arranque, sube de valor cada vez que pasaron 15 vueltas sin problemas de inyeccion

int CINY_RPM = 0;
int CINY_X;					//la uso para redondear rpm para rpm objetivo de acl

bool tempserv = false;      //en true si el motor alcanzo la temperatura de funcionamiento
//mega tabla de valores de inyeccion
const int T0[18][12] ={
  /*Carga ------>  0 18 27 36 45 55 64 73 82 91 100*/
/*RPM ---> 0800*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*1000*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*1200*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*1500*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*2000*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*2200*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*2500*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*2700*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*3000*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*3200*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*3500*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*3700*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*4000*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*4500*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*5000*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*5500*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
         /*6000*/{0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

  /*-----( Variables Encendido )-----*/

  int avanceDeChispa        = 3; //en dientes de volante (2,42 grados) , default en 3
  int periodo               = 7; //periodo en mS
  unsigned long tiempo      = 0;
  unsigned long millisant   = 0;
  int pinBobinas14          = 8; //pines utilizados para mandar señales a las bobinas
  int pinBobinas23          = 9;
  bool activado             = false; //?
  bool arrancando           = false; //Esta arrancando?
  int tablaAvance[18][11]     ={/*RPM|TEMP   0 ;18;27;36;45;55;64;73;82;91;100 */
								/*800*/		{3 ,3 ,3 ,3 ,4 ,6 ,6 ,7 ,8 ,8 ,8 },
								/*1000*/	{3 ,3 ,3 ,4 ,5 ,8 ,8 ,8 ,10,10,10},
								/*1200*/	{5 ,5 ,5 ,5 ,6 ,9 ,9 ,10,11,12,12},
								/*1500*/	{8 ,8 ,8 ,8 ,8 ,10,10,11,13,14,14},
								/*1700*/	{10,10,10,10,10,11,11,12,14,15,15},
								/*2000*/	{12,12,12,12,12,13,13,14,15,16,16},
								/*2200*/	{13,14,14,14,14,15,15,16,17,18,18},
								/*2500*/	{15,16,16,16,16,17,17,17,18,19,19},
								/*2700*/	{17,18,18,18,18,19,19,19,20,21,21},
								/*3000*/	{19,20,20,20,20,21,21,21,22,23,23},
								/*3200*/	{21,22,22,22,22,23,23,23,24,25,25},
								/*3500*/	{23,24,24,24,24,25,25,25,26,27,27},
								/*3700*/	{25,26,26,26,26,27,27,27,28,29,29},
								/*4000*/	{26,28,28,28,28,29,29,29,30,31,31},
								/*4500*/	{28,30,30,30,30,31,31,31,32,33,33},
								/*5000*/	{30,32,33,33,33,34,34,34,35,36,36},
								/*5500*/	{32,34,34,34,34,34,34,35,37,38,38},
								/*6000*/	{34,35,35,35,35,35,35,36,38,39,39}
                                };				 //matriz tabla de avance
                                
/*-----( Variables Temperatura )-----*/
int  sensorT   = A0; //pin de sensor de temperatura
int  temp      = 20; //temperatura para LCD


/*-----( Variables LCD )-----*/
Adafruit_LiquidCrystal lcd(0x27);

const uint8_t charBitmap[][8] = { 
    //array con simbolo de grados en diferentes lugares :P
   { 0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0 },
   { 0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0 },
   { 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0 } 
};


/*-----( Variables Logica )-----*/
int i = 0;
int var  = 0;               //variable usada para bucles
int varX = 0;               //variable temporal, multiples usos
int pinLuzMuerte = 12;       //sip, pin de luz de "check Engine"

/*-----( Variables Comunicacion )-----*/
bool _msg = false;          //si esta en true hay un mensaje nuevo del puerto serie
String msg = "";            //Mensaje que llego por serie
long MPER  = 500;           //cantidad de ms que esperamos antes de enviar un mensaje con la info del motor
long _PANT = 0;             //valor de millis() del mensaje anterior

void setup(){

     lcd.begin(20, 4);
     Serial.begin(9600);
	 //Iniciamos "hilos"
	 Scheduler.startLoop(HILO_1);
	 Scheduler.startLoop(HILO_2);
	 Scheduler.startLoop(HILO_3);
	 //interrupcion rpm
	 attachInterrupt(0, I_RPM, FALLING);
	 //Configuramos E/S
     pinMode(sensorT, INPUT);
     pinMode(iny[0] ,OUTPUT);
     pinMode(iny[1] ,OUTPUT);
     pinMode(iny[2] ,OUTPUT);
     pinMode(iny[3] ,OUTPUT);
     pinMode(pinLuzMuerte, OUTPUT);

     //grabamo todo en la flashhh ameo (bah en la v1.8 que va a tener reprogramacion por pc sin tener que subir el programa devuelta)
}

void loop(){
    //este hilo se encarga de la comunicacion con pc y programacion
    if(_msg){ //Nos llego un mensaje desde la pc y nos ponemo re felices :3 XDD
       

       
       
        msg  = ""; //vaciamo la variable apra que no rompa las bola :3
        _msg = false;
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
	sinc(1,sensortemp());
	T_RPM_AC = micros();
	if (T_RPM_AC - T_RPM_A >= RPM_per) {
		 detachInterrupt(0); //asesinamos las interrupciones mientras calculamos rpm
         T_RPM_A = T_RPM_AC;
		 int _X_RPM = (_PR / dnt)*90; //calculo para obtener las rpm
		 sinc(0,_X_RPM);
		 attachInterrupt(0, I_RPM, FALLING);
    }
    
}
void I_RPM(){ //interrupcion para rpm
    _PR++;
    Tdnt[i2] = micros() - Tant;
    Tant = micros();
    i2++;
    dntact++;
    if (i2 < dnt){ 
        detachInterrupt(0);
        dntact = 0;
        sincronizar();
        i2 = 0;} //reseamos la variable por cada vuelta que pasa
}

void HILO_2(){
	//C INY / Fixed Mode
    C_PWM(0);
}
//Funciones que dependen del hilo2 :
void ControlINY(){ //Controla el tiempo de inyeccion
	CINY_RPM = _RPM;
	if(acl = false){
        //*#*#*# Bloque de control de inyeccion cuando se intenta arrancar el motor
		if(CINY_RPM <= ARR_FLAG2 && CINY_RPM != 0){
			//ARR_FRIO(); //cambiar luego con arranque teniendo en cuenta temperatura
        }
        //#####* Bloque de control de motor normal
        if(CINY_RPM >= ARR_FLAG2 && CINY_RPM != 0){
			 inyT = Tiny(CINY_RPM, marv); 
			 CINY_X = map(_RPM,0,7000,0,70);
             CINY_X = CINY_X * 100;
             if(tempserv){
             
             }
			 acl = ACL(CINY_X, inyT);
		} 
	}else{acl = ACL(CINY_X, inyT);}
}


bool ACL(int rpmOBJ, int TBASE){
	int ACL_RPM = _RPM - rpmOBJ;
	if(ACL_RPM >= 400 &&  vuelta2 > ACL_PER){ //diferencia mayor a 400 vueltas
		TBASE = TBASE - ACL_TMP; // sacamos 1.5mS de tiempo de inyeccion
		vuelta2 = 0;
		return true;
	} else if(ACL_RPM <= 400 && vuelta2 > ACL_PER){
		TBASE = TBASE + ACL_TMP;
		vuelta2 = 0;
		return true;
	}else {// ya no hace falta cambiar el tiempo desde esta funcion, esto habilita correccion por sonda lambda y TMAP
		acl = false;
		vuelta2 = 0;
		return false;} 
	}

int Tiny(int rpm2, int marv2){
	int rpm3  = map(rpm2,0,7000,0,17); //aproximamos las rpm
	int marv3 = map(marv2,0,255,0,10); //aproximamos el valor de la mariposa de aceleracion
    int tiempo;
    return tiempo;
}

int Tiny_C(int _op , int _V){ //_op = opcion , _V valor base inyeccion
     //tiempo de inyeccion compensado por TMAP / sonda lambda
    return _V;
}


void C_PWM(int PWM_T){
    //nuevo control del pwm de los inyectores
    int _C_PER = 0; //la uso para recorrer el array de injectores
    int _C_X = dntPER - dntIAVC; // restamos el periodo al avance
    if(_C_X <= dntact){
        dntact = 0; //reseteamos variabe para no hacer mas quilombo de logica al p2
        digitalWrite(iny[_C_PER], HIGH);
        delayMicroseconds(PWM_T); //delay a lo rubio salvaje :V (Scheduler usa el tiempo que esta en delay a para ejecutar otros hilos)
        digitalWrite(iny[_C_PER], LOW);
        _C_PER++;
    }
    _C_X = cilindros - 1;
    if(_C_PER <= _C_X){
        _C_PER = 0;
    }
}

void HILO_3(){
	//control encendido
}

#if (motor == 1)
//------------------------(FrancisJPK inicio)-----------------------------------------
void controlDeEncendido(float temperatura){

    //----CONTROL DE AVANCE ANTES DE MANDAR CHISPA----
    if(arrancando == true){
		//controlador de avance en frio
        avanceArranque();
    }else if(arrancando == false){
		avanceNormal(temperatura);
    }
    //---------------FIN DE CONTROL DE AVANCE PARA PROCEDER A MANDAR CHISPA-----------
	//--------------------------------------------------------------------------------
    //----------------------------------CONTROL DE MANDADA DE CHISPA-----------------
        if(_POS >= ((dnt/4) - avanceDeChispa) && _POS <= ((dnt/4)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas14);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 1 y 4(siendo el 4 chispa perdida)
        }else if(_POS >= ((dnt/2) - avanceDeChispa) && _POS <= ((dnt/2)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas23);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 2 y 3(siendo el 2 chispa perdida)
        }else if(_POS >= (((dnt/4)*3) - avanceDeChispa) && _POS <= (((dnt/4)*3)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas14);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 1 y 4(siendo el 1 chispa perdida)
        }else if(_POS >= (dnt - avanceDeChispa) && _POS <= (dnt - avanceDeChispa)+1 ){
            iniciarChispazo(pinBobinas23);
            millisant = millis();
            activado = true;
            //----chispazo para el piston 2 y 3(siendo el 3 chispa perdida)
        }

        if ((millis() - millisant) >= periodo && activado == true) {
            millisant = millis();
            activado = false;
            pararChispazo(pinBobinas14);
            pararChispazo(pinBobinas23);
        }
}//------------------FIN DE CONTROL DE MANDADA DE CHISPA-------------------------
//------------------------------------------------------------------------------
//--------------------BLOQUE DE CONTROLADORES DE AVANCE--------------------------

void avanceArranque(){
	//avanceAnterior = avanceDeChispa; FDSoftware: da error despue fijate XD
	avanceDeChispa = dientes(3);
	//if(avanceAnterior != avanceDeChispa){
	//	LCD(2,avanceDeChispa,0);
	//}
}
void avanceNormal(float temperatura){

	int indiceTemp = 0;//indice que indica a que columna de la matriz acceder(temperatura)
	int indiceRPM = 0;//indice que indica a que fila de la matriz acceder(RPM)

	//------------BLOQUE DE ASIGNACION DE INDICES A LEER EN TABLAS------------ 
	//if{temperatura < 98} //FDSoftware: weeeey los if van con () no con {} XD
    if(temp < 98){
		indiceTemp = ((temperatura < 18)? 0:((temperatura < 27)? 1:((temperatura < 36)? 2:((temperatura < 45)? 3:((temperatura < 55)? 4:((temperatura < 64)? 5:((temperatura < 73)? 6:((temperatura < 82)? 7:((temperatura < 91)? 8:9)))))))));
		//este bodoque gigante de operaciones ternarias es para asignar el indice a leer en tabla segun la temperatura
		//este indice corresponde a las columnas (eje X)
	}else{
		indiceTemp = 10;
	}
	indiceRPM = ((_RPM<1000)?0:((_RPM<1200)?1:((_RPM<1500)?2:((_RPM<1700)?3:((_RPM<2000)?4:((_RPM<2200)?5:((_RPM<2500)?6:((_RPM<2700)?7:((_RPM<3000)?8:((_RPM<3200)?9:((_RPM<3500)?10:((_RPM<3700)?11:((_RPM<4000)?12:((_RPM<4500)?13:((_RPM<5000)?14:((_RPM<5500)?15:((_RPM<6000)?16:17))))))))))))))))); 
	//este otro bodoque gigante de operaciones ternarias es para asignar el indice a leer en tabla segun las RPM
	//este indice corresponde a las filas (eje Y)
	//---------
	//avanceAnterior = avanceDeChispa; FDSoftware: RT :V, marca error de que no la declaraste :V
	avanceDeChispa = dientes(tablaAvance[indiceRPM][indiceTemp]);
	//finalmente accedemos al valor en tabla correspondiente al estado actual del motor
	//if(avanceAnterior != avanceDeChispa){ RT :,v
	//	LCD(2,avanceDeChispa,0);
	//}
	//-----------FIN DEL BLOQUE DE ASIGNACION DE INDICES A LEER EN TABLAS-----
}
//--------------------FIN DE BLOQUE DE CONTROLADORES DE AVANCE----------------
//-----------------------------------------------------------------------------
//---------------------FUNCIONES DE CONTROL DE BOBINAS------------------------

#endif
void iniciarChispazo(int pin){//inicia la chispa hasta que se llame al metodo pararChispazo()
    digitalWrite(pin, HIGH);
}
void pararChispazo(int pin){//para la chispa
    digitalWrite(pin, LOW);
}
//---------------------(FrancisJPK final)---------------------------------------------------

int  sensortemp(){
  //esta funcion mide la temperatura y devuelve int en celsius
  //**solo para sensor LM35**
    int value = analogRead(sensorT);
    int millivolts = (value / 1023.0) * 5000;
    int celsius = millivolts / 10; 
    return celsius;
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

void sincronizar(){
    #if(dev == 1) 
        if (sincronizado == false){
            Serial.println("DBG Sincronizando PMS con inyeccion y encendido");
        }
   #endif
    //Esta funcion sincroniza el valor de "vuelta" con el PMS del piston 1
    int TMIN = 99999; //almacena el tiempo menor de cada diente
    int IMIN = 0; //almacena el indice del menor tiempo
    int TMAX = 0; //almacena el tiempo mayor (Este seria teoricamente el PMS del piston 1)
    int IMAX = 0; //almacena el indice del diente con PMS del piston 1, luego se restablece la variable diente...
    //a 0 y se pone la variable sincronizado en true para habilitar el resto del programa.
    int i2   = 0; //variable para recorer array de tiempos
    for(int i = 0; i < dnt; i++){ //i menor a dnt porque empieza la cuenta en 0 el for y los dientes no
        if (Tdnt[i2] < TMIN){
            TMIN = Tdnt[i2];}
        if (Tdnt[i2] > TMAX){
            TMIN = Tdnt[i2];}
        i2++;
    }
    attachInterrupt(0, I_RPM, FALLING);
    sincronizado = true;
}


//funcion para el control del lcd
//modificar luego para actualizar solo valores que cambian no toda la linea
void LCD(int op, float texto, int texto2){
    String linea1 = "###OpenEFI v0.010###";
    String lin2  = "#RPM: ";
    String lin2a = "#T:";
    String lin2b = "#";
    String lin3a = "###T.INY:";
    String lin3b = " mS###";
    String lin4a = "###Avance: ";
    String lin4b = "###";
    String LX;
    if(op == 0){
         LX = lin2 + texto2 + lin2a + temp;
        lcd.setCursor(0,1);
        lcd.print(LX);
        lcd.setCursor(17,1);
        lcd.print (char(random(7)));
        lcd.setCursor(18,1);
        lcd.print("C#");
        lcd.print(LX);
    }
    if(op == 1){
         LX = lin3a + texto + lin3b;
        lcd.setCursor(0,2);
        lcd.print(LX);
    }
    if(op == 2){
        LX = lin4a + texto;
        lcd.setCursor(0,3);
        lcd.print(LX);
        lcd.setCursor(16,3);
        lcd.print (char(1));
        lcd.setCursor(17,3);
        lcd.print(lin4b);
    }
    if(op == 5){ //esta opcion es para inciar el panel
        lcd.setCursor(0,0);
        lcd.print("###OpenEFI v0.1.5##");
        lcd.setCursor(0,1);
        lcd.print("# RPM:0000 #T 000");
        lcd.setCursor(17,1);
        lcd.print (char(1));
        lcd.setCursor(18,1);
        lcd.print("C#");
        lcd.setCursor(0,2);
        lcd.print("### T.INY: 00  mS###");
        lcd.setCursor(0,3);
        lcd.print("#### Avance: 00");
        lcd.setCursor(16,3);
        lcd.print (char(random(7)));
        lcd.setCursor(17,3);
        lcd.print(lin4b);
    }
}

//convierte grados en dientes del sensor hall
int dientes(float grados){
    float grad = 360 / dnt; //dividimos 360 grados por la cantidad de dientes
    //dividimos grados por grad, luego multiplicamos por 100 para transformar el float en int
    int x2     = (grados / grad) * 100; 
    //dividimos por 100, al hacer esto se eliminan los decimales, en prox ver redondear
    int dnt2    = x2 / 100;
    return dnt2; 
}

//funcion en caso de emergencia del motor
void emerg(){
    #if(dev == 1) 
        Serial.println("DBG Motor en estado de emergencia");
   #endif
    noInterrupts(); //desactivamos interrupciones
    digitalWrite(pinLuzMuerte, HIGH);
    int varemerg = 1;
    lcd.setCursor(0,0);
    lcd.print("####################");
    lcd.setCursor(0,1);
    lcd.print("#### EMERGENCIA ####");
    lcd.setCursor(0,2);
    lcd.print("## Motor detenido ##");
    lcd.setCursor(0,3);
    lcd.print("####################");
    while(varemerg == 1){
        delay(1000);
    }
}