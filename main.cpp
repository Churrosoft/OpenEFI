//Programa principal
//Creado por FDSoftware y FrancisJPK

/*-----( Importar Librerias )-----*/
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <TMAP.h>
/*-----( Declarar variables :D )-----*/

#define motor 1 //definir tipo de motor 0 = diesel ; 1 = nafta
const String ver =  "## ver 0.01 Alpha ##";
/*-----( Variables INYECCION )-----*/

int iny[] = {3,4,5,6};       //Pines de arduino que esta conectados los inyectores **CAMBIAR PINES**
int tMax     = 22;           //tiempo maximo de inyeccion
int perinyec = 5;            //cantidad de dientes que tiene que detectar el sensor hall para inyectar
bool per = false;            //usada para no inyectar mas de una vez en el mismo cilindro

float inyT   = 1500;         //tiempo de inyeccion uS (actual)
float inyTR  = 1250;         //tiempo de inyeccion Ralenti (uS)
float inyTARR= 22000;        //tiempo de inyeccion Arranque
unsigned long tmpINY = 0;     //tiempo que paso desde activacion de inyector

int varINY   = 0;            //varible temporal, guarda ultimo inyector activado

int Lamb     = A0;           //pin de sonda lambda
int ARRpin   = 10;           //pin conectado que recibe señal de arranque de llave
bool arr     = false;        //si se intenta arrancar el motor esta en true

int mar      = A1;           //pin de mariposa de acelerador
int marv     = 0;            //valor actual de mariposa de acelerador
bool inyectando = false;        //variable para saber si se esta inyectando todavia o no

bool acelerar;               //varible temporar, guarda si se intento acelerar el motor
int vuelta2  = 0;            //numero de vuelta para inyeccion/encendido
int vuelta3  = 0;            //numero de vuelta para inyeccion/encendido
/*-----( Variables RPM )-----*/

int intervalo1 = 500;        //intervalo para medir rpm
int pul        = LOW;        //pulso de disparo de sensor de posicion de cigueñal
bool varrpm    = LOW;        //variable para no volver a contar mismo diente como pulso

int pinrpm     = 10;         //pin conectado al sensor de posicion de cigueñal
int diente     = 0;          //numero de diente actual de la corona
int dnt        = 20;         //cantidad de dientes que detecta el sensor hall

int rpm        = 0;          //numero actual de rpm
int rpmMIN     = 800;        //numero minimo de rpm
int rpmMAX     = 7000;       //numero maximo de rpm

int tolrpm     = 50;         //tolerancia de rpm
int vuelta     = 0;          //numero de vuelta para rpm
bool varr      = false;      //variable para evitar calcular rpm varias veces
int rpmant     = 0;          //rpm anteriores

int promedio   = 0;          //promedio tiempo entre dientes
int Tdnt[11];                //aca se guardan los tiempos entre dientes, luego se promedia (son 12);

unsigned long milirpm;       //tiempo previo ultimo periodo medicion rpm
unsigned long curMillis;     //tiempo actual del micro

bool sincronizado = false;   //en true si esta sincronizado la variable vuelta con el PMS
/*-----( Variables Temperatura )-----*/

int sensorT    = 3;          //pin de sensor Temperatura
int pinVENT    = 13;         //pin de control del ventilador

float temp     = 0;          //temperatura motor
float tempfrio = 40;         //temperatura maxima para considerar el motor "frio"
float tempmax  = 115;        //temperatura maxima de funcionamiento antes de entrar en modo emergencia

float tempvenMIN = 80;      //temperatura a la que se apaga el ventilador
float tempvenMAX = 95;      //temperatura a la que se enciende el ventilador

/*-----( Variables Encendido )-----*/


  int avanceDeChispa     = 3;      //en dientes de volante (2,42 grados) , default en 3
  int periodo          = 7;      //periodo en mS
  unsigned long tiempo   = 0;
  unsigned long millisant   = 0;
  int pinBobinas13         = 8;
  int pinBobinas24         = 9;
  bool activado = false;
  bool arrancando = false;

/*-----( Variables Logica )-----*/
int var = 0;                //variable usada para bucles
int varX = 0;               //variable temporal, multiples usos

/*-----( Otras variables )-----*/
TMAP myTMAP = TMAP(A2,A3,1.75,2.10); //iniciamos sensor TMAP
int pinLuzMuerte = 12;       //sip, pin de luz de "check Engine"
bool emergencia  = false;    //mientras que este en false todo bien ^~^
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // definimos el display I2C
const uint8_t charBitmap[][8] = { //array con simbolo de grados en diferentes lugares :P
   { 0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0 },
   { 0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0 },
   { 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0 },
   { 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0, 0x0 },
   { 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0x0 },
   { 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0x0 },
   { 0x0, 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0x0 },
   { 0x0, 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0x0 }
   
};
void int0(){
    if(emergencia == false){ //si entro en modo emergencia no hacemos caso a nada :V
        varr = false;
        vuelta++;
        vuelta3++;
        if(vuelta >= perinyec){
              vuelta2++;
              per = true;
        }
    }
}

void setup(){
    //definir E/S
    pinMode(pinrpm, INPUT);
    pinMode(sensorT, INPUT);
    pinMode(mar, INPUT);
    pinMode(Lamb, INPUT);
    pinMode(ARRpin, INPUT);
    pinMode(pinLuzMuerte, OUTPUT);
    pinMode(pinVENT, OUTPUT);

    int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));
  #if (motor == 1)
    pinMode(pinBobinas13, OUTPUT);
    pinMode(pinBobinas24, OUTPUT);
    //definimos estado de las bobinas
    digitalWrite(pinBobinas13, LOW);
    digitalWrite(pinBobinas24, LOW);
  #endif
    for(var = 0; var >= 3; var++){
    //definimos inyectores como salida
        pinMode(iny[var], OUTPUT);
    }

    for(var = 0; var >= 3; var++){
        digitalWrite(iny[var], LOW);
    }
    //definir interrupciones HW
     attachInterrupt(digitalPinToInterrupt(2), int0, RISING);
     //mostramos mensaje en LCD :D
    lcd.setCursor(0,0);
    lcd.print("####################");
    lcd.setCursor(0,1);
    lcd.print("##### OpenEFI ######");
    lcd.setCursor(0,2);
    lcd.print(ver);
    lcd.setCursor(0,3);
    lcd.print("####################");
    delay(500);
    LCD(5,0,0);
}

void loop(){
    for(;;){
        ControlRPM();
        marv = analogRead(mar) / 4;
        temp = sensortemp();
        Temperatura();
        bool varINY = ControlARR(); //control de arranque del motor
        ControlINY(varINY);
        controlDeEncendido(temp);
        ControlPWM();
    }
}

void ControlRPM(){  
int dntA   = 0; //numero de diente anterior 
int indice = 0; //indice para guardar en array de Tdnt
int rpmtT  = 0; //variable temporal para calcular rpm

  // codigo para sincronizar el cigueñal
      if(varr == false){
         milirpm = millis();
         varr = true;
		 dntA = vuelta;
     }
  if(indice >= 11){
     if(varr == true && dntA != vuelta){ //si se detecto un diente nuevo y se guardo el tiempo anterior 
      Tdnt[indice] = millis() - milirpm;
      varr = false;
	  indice++;
     }
  }else{
	sincronizar();
	indice = 0;
  }
  //calculamos las rpm promediando el tiempo de 4 dientes
  if (indice <= 3){
	rpmT = (Tdnt[0] + Tdnt[1] + Tdnt[2] + Tdnt[3]) / 4; //sacamos el tiempo promedio de los 4
	rpmT = 1000 / rpmT; //divimos el tiempo por 1000 para tener la cantidad de dientes por segundo
	rpmT = dnt /rpmT;   //ahora tenemos la cantidad de vueltas por segundo
	rpm = rpmT * 60;    //finalmente tenemos las RPM
  }
}

void ControlINY(bool varY){
    //Controla los tiempos de inyeccion dependiendo la mariposa,rpm y temperatura del motor
    int tempX; //variable temporal
    int tempX2;//variable temporal
    int tempX3;//variable temporal, luego dejar una sola
    if(varY == true){
        //sumamos y restamos la tolerancia a las rpm minimas
        tempX  = rpmMIN + tolrpm;
        tempX2 = rpmMIN - tolrpm;
        //sumamos la tolerancia a las rpm maximas
        tempX3 = rpmMAX + tolrpm;
     if(rpm <= tempX2 && acelerar == false && marv >= 15){ //si las rpm caen debajo las rpm minimas y no esta presionado acelerador
              if(temp >= tempfrio){
                  inyT = inyT + 700; //sumamos 700uS / 0.7mS si el motor esta frio
             }else{
                  inyT = inyT + 500; //restamos 500 uS/ 0.5mS si el motor esta en temperatura de funcionamiento
              }
              acelerar = true;
              rpmant = rpm;
           }

           if(rpm >= tempX && acelerar == false && marv >= 15){ //si las rpm pasan las rpm minimas y no esta presionado acelerador
              if(temp >= tempfrio){
                   inyT = inyT - 700; //restamos 700uS / 0.7mS si el motor esta frio
             }else{
                  inyT = inyT - 500; //restamos 500 uS/ 0.5mS si el motor esta en temperatura de funcionamiento
             } 
             acelerar = true;
             rpmant = rpm;
          }
          if(marv >= 10 && rpm <= 3000){
              //cortamos inyeccion si las rpm pasan las 3000 y se solto el acelerador
              inyT = 0;
          }
          if(marv >= 10 && rpm <= 1500){
              //volvemos a habilitar la inyeccion a las 1500 vueltas
              inyT = inyTR + 200;
          }
       int varxx = rpm - rpmant;
        if(vuelta3 >= (dnt * 10) && varxx <= 15){
           // si no hubo un cambio mayor a la 15rpm luego de 10 vueltas de cigueñal volver a corregir
           acelerar = false;
            vuelta3 = 0;
      }
    }
}
void ControlPWM(){
    if(emergencia == false){
        if (inyectando == false  && per == true){
          digitalWrite(iny[vuelta2++],HIGH);
            unsigned long tmpINY = micros();
            inyectando = true;
            per = false; //per es para controlar que se inyecte una sola vez :P
        }
      if (micros() - tmpINY >= inyT) {
        digitalWrite(iny[vuelta2++],LOW);
        if(vuelta2 == 3){
          vuelta2 == 0;
        }
        inyectando == false;
      }
    }
}


#if (motor == 1)
void controlDeEncendido(float temperatura){

    //----bloque que controla el avance antes de hacer la chispa----
    if(arrancando == true){
        ControlEncendidoArranque(temperatura);
    }else if(arrancando == false){
        if(temperatura < 60){
            ControlEncendidoFrio();
        }else{
            ControlEncendidoNormal();
        }
    }
    //-------fin del bloque de control de avance-----------
    //--------inicio del bloque de control de cuando se larga la chispa------------
        if(diente >= ((dnt/4) - avanceDeChispa)&&diente <= ((dnt/4)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas13);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 1 y 3(siendo el 3 chispa perdida)
        }else if(diente >= ((dnt/2) - avanceDeChispa)&&diente <= ((dnt/2)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas13);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 1 y 3(siendo el 1 chispa perdida)
        }else if(diente >= (((dnt/4)*3) - avanceDeChispa)&&diente <= (((dnt/4)*3)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas24);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 2 y 4(siendo el 2 chispa perdida)
        }else if(diente >= (dnt - avanceDeChispa)&&diente <= (dnt - avanceDeChispa)+1 ){
            iniciarChispazo(pinBobinas24);
            millisant = millis();
            activado = true;
            //----chispazo para el piston 2 y 4(siendo el 4 chispa perdida)
        }

        if ((millis() - millisant) >= periodo && activado == true) {
            millisant = millis();
            activado = false;
            pararChispazo(pinBobinas13);
            pararChispazo(pinBobinas24);
        }
    //------fin del bloque que controla la mandada de chispa------------------
}
//---------------BLOQUE DE CONTROL DE AVANCE
void ControlEncendidoFrio(){
    if(rpm < 1500&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(5);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 2200&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(7);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 3000&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(9);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 3500&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(11);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 4000&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(17);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 5000&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(22);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 6000&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(26);
        LCD(2,avanceDeChispa,0);
    }
}
void ControlEncendidoNormal(){
    if(rpm < 1500&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(7);
        LCD(2,avanceDeChispa,0);// esto esta bien asi?
    }else if(rpm < 2200&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(12);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 3000&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(15);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 3500&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(18);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 4000&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(23);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 5000&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(30);
        LCD(2,avanceDeChispa,0);
    }else if(rpm < 6000&&avanceDeChispa != avanceAnterior){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(37);
        LCD(2,avanceDeChispa,0);
    }
}
void ControlEncendidoArranque(float temperatura){
    if(temperatura < 45){
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(4);
        LCD(2,avanceDeChispa,0);
    }else{
        avanceAnterior = avanceDeChispa;
        avanceDeChispa = dientes(6);
        LCD(2,avanceDeChispa,0);
    }
}
//--------------------FIN DEL BLOQUE DE CONTROL DE AVANCE
//-------------------FUNCIONES DE CONTROL DE BOBINAS
void iniciarChispazo(int pin){//inicia la chispa hasta que se llame al metodo pararChispazo()
    digitalWrite(pin, HIGH);
}
void pararChispazo(int pin){//para la chispa
    digitalWrite(pin, LOW);
}
//---------------
#endif

void Temperatura(){
    //Controla el ventilador y apagado de emergencia del motor
    if(temp >= tempmax){
        inyT = 0;
        emergencia = true;
        emerg();
    }
    if(temp <= tempvenMIN){
        digitalWrite(pinVENT, LOW);
    }
    if(temp >= tempvenMAX){
        digitalWrite(pinVENT, HIGH);
    }
}


bool ControlARR(){
    //se fija si se esta arrancando el motor, fija la inyeccion,
    //pone el avance en modo arranque y desactiva sonda lamba,acelerador y control de rpm ralenti
    //hasta que las rpm sean mayores a 1000~
    int senalarr = digitalRead(ARRpin);
    if (senalarr == HIGH && vuelta2 <= 10){ //se esta intentando arrancar el motor y pasaron menos de 10 vueltas
        //usar cantidad de vueltas o tiempo y rpm para saber si se acelero o no ?)
    #if (motor == 1)
        ControlEncendidoArranque(temp);
    #endif
        arrancando = true;
        arr = true;
        inyT = inyTARR;
        return false;
    }if (senalarr == LOW && rpm >= rpmMIN) {
    #if (motor == 1)
        arrancando = false;
    #endif
        return true; //habilitamos el control de inyeccion 
    }
}


float  sensortemp(){
  //esta funcion mide la temperatura y devuelve float en celsius
    int value = analogRead(sensorT);
    float millivolts = (value / 1023.0) * 5000;
    float celsius = millivolts / 10; 
    return celsius;
} 
//funcion para el control del lcd
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
        lcd.print("###OpenEFI v0.010###");
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
    dientes    = x2 / 100;
    return dientes; 
}
//funcion en caso de emergencia del motor
void emerg(){
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

void sincronizar(){
    //Esta funcion sincroniza el valor de "vuelta" con el PMS del piston 1
}