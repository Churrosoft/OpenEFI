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

void setup(){
    lcd.begin(20, 4);
	 #if(dev == 1)
		 Serial.begin(9600);
	 #endif
	 //Iniciamos "hilos"
	 Scheduler.startLoop(HILO_1);
	 Scheduler.startLoop(HILO_2);
	 Scheduler.startLoop(HILO_3);
	 //interrupcion rpm
	 attachInterrupt(0, I_RPM, FALLING);
	 //Configuramos E/S
	 pinMode(sensorT, INPUT);
}

void loop(){
	//este hilo se encarga de la comunicacion con pc y programacion
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
    if (i2 < dnt){ 
        detachInterrupt(0);
        sincronizar();
        i2 = 0;} //reseamos la variable por cada vuelta que pasa
}

void HILO_2(){
	//C INY / Fixed Mode
}

void HILO_3(){
	//control encendido
}

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