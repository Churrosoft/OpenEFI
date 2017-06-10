//Programa principal
//Creado por FDSoftware y FrancisJPK

/*-----( Importar Librerias )-----*/
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
/*-----( Declarar variables :D )-----*/

#define motor 1 //definir tipo de motor 0 = diesel ; 1 = nafta

/*-----( Variables INYECCION )-----*/

int iny[] = {3,4,5,6};       //Pines de arduino que esta conectados los inyectores **CAMBIAR PINES**
int tMax     = 22;           //tiempo maximo de inyeccion
int perinyec = 5;            //cantidad de dientes que tiene que detectar el sensor hall para inyectar

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

unsigned long milirpm;    //tiempo previo ultimo periodo medicion rpm
unsigned long curMillis;     //tiempo actual del micro
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
int pinLuzMuerte = 12;       //sip, pin de luz de "check Engine"
bool emergencia  = false;    //mientras que este en false todo bien ^~^
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // definimos el display I2C

void int0(){
    //if(emergencia == false){ //si entro en modo emergencia no hacemos caso a nada :V
        varr = true;
        vuelta++;
        vuelta3++;
          if(vuelta >= perinyec){
              vuelta2++;
        }
   // }
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
    lcd.print("##### OPENEFI ######");
    lcd.setCursor(0,2);
    lcd.print("## ver 0.01 Alpha ##");
    lcd.setCursor(0,3);
    lcd.print("####################");
    delay(500);
    LCD(5,4);
}

void loop(){
        ControlRPM();
        marv = analogRead(mar);
        temp = sensortemp();
        Temperatura();
        bool varINY = ControlARR(); //control de arranque del motor
        ControlINY(varINY);
        controlDeEncendido(temp);
        ControlPWM();
}

void ControlRPM(){    
   //calcula rpm cada 1/4 vuelta del cigueñal
      if(varr == false && vuelta == 0){
    milirpm = millis();
    varr = true;
     }
  
     if(varr == true && vuelta == 50){
      rpm = ((millis() - milirpm)* 4 )* 60000;
      varr = false;
     }
}

void ControlINY(bool varY){
    //Controla los tiempos de inyeccion dependiendo la mariposa,rpm y temperatura del motor
    if(varY == true){
     if(rpm <= rpmMIN && marv <= tolrpm && acelerar == false){ //si las rpm caen debajo las rpm minimas y no esta presionado acelerador
              if(temp >= tempfrio){
                  inyT = inyT + 700; //sumamos 700uS / 0.7mS si el motor esta frio
             }else{
                  inyT = inyT + 500;
              }
              acelerar = true;
              rpmant = rpm;
           }

           if(rpm >= rpmMIN && marv <= tolrpm && acelerar == false){ //si las rpm pasan las rpm minimas y no esta presionado acelerador
              if(temp >= tempfrio){
                   inyT = inyT - 700; //restamos 700uS / 0.7mS si el motor esta frio
             }else{
                  inyT = inyT - 500; //restamos 500 uS/ 0.5mS
             } 
             acelerar = true;
             rpmant = rpm;
          }
       int varxx = rpm - rpmant;
        if(vuelta3 >= (dnt * 10) && varxx <= 5){
           // si no hubo un cambio mayor a 5 rpm luego de 10 vueltas de cigueñal volver a corregir
           acelerar = false;
            vuelta3 = 0;
      }
    }
}
void ControlPWM(){
    if(emergencia == false){
        if (inyectando == false){
          digitalWrite(iny[vuelta2++],HIGH);
            unsigned long tmpINY = micros();
          inyectando = true;
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
    //----
    //--------inicio del bloque de control de cuando se larga la chispa------------
        //FDSoftware: mandale un mayor o igual, por si se atrasa en algo el arduino y no llega a ejecutar esta parte :P
        if(diente == (33 - avanceDeChispa) ){//----chispazo para el piston 1 y 3(siendo el 3 chispa perdida)
          iniciarChispazo(pinBobinas13);//iniciar chispazo
            activado = true;
            //FDSoftware: el control para sacar la chispa va afuera del if de dientes wey XDD
            //sino va a comprobar si paso el tiempo recieen la proxima vez que pase por el diente del if :P

            //esperar un tiempito
            if(activado == true){
                tiempo = millis();
            }
            
            if ((millis() - millisant) >= periodo && activado == true) {
                pararChispazo(pinBobinas13);//una vez pasados 5ms terminar chispazo
                millisant = millis();
                activado = false;
            }
        }else if(diente == (66 - avanceDeChispa)){//----chispazo para el piston 1 y 3(siendo el 1 chispa perdida)
            iniciarChispazo(pinBobinas13);
            activado = true;
            //esperar un tiempito
            if(activado == true){
                tiempo = millis();
            }
            if ((millis() - millisant) >= periodo && activado == true) {
                pararChispazo(pinBobinas13);
                millisant = millis();
                activado = false;
            }
        }else if(diente == (99 - avanceDeChispa)){//----chispazo para el piston 2 y 4(siendo el 2 chispa perdida)
            iniciarChispazo(pinBobinas24);
            activado = true;
            //esperar un tiempito
            if(activado == true){
                tiempo = millis();
            }
            if ((millis() - millisant) >= periodo && activado == true) {
                pararChispazo(pinBobinas24);
                millisant = millis();
                activado = false;
            }
        }else if(diente == (132 - avanceDeChispa)){//----chispazo para el piston 2 y 4(siendo el 4 chispa perdida)
            iniciarChispazo(pinBobinas24);
            activado = true;
            //esperar un tiempito
            if(activado == true){
                tiempo = millis();
            }       
            if ((millis() - millisant) >= periodo && activado == true) {
                pararChispazo(pinBobinas24);
                millisant = millis();
                activado = false;
            }
        }
    //------fin del bloque que controla la mandada de chispa------------------
}
//---------------
void ControlEncendidoFrio(){
    if(rpm < 2000){
        avanceDeChispa = 4;//+-10°
    }else if(rpm < 3000){
        avanceDeChispa = 6;//+-15°
    }else if(rpm < 4000){
        avanceDeChispa = 8;//+-20°
    }else if(rpm < 5000){
        avanceDeChispa = 10;//+-25°
    }else if(rpm < 6000){
        avanceDeChispa = 12;//+-30°
    }
}
void ControlEncendidoNormal(){
    if(rpm < 2000){
        avanceDeChispa = 4;//+-10°
    }else if(rpm < 3000){
        avanceDeChispa = 7;//+-17,5°
    }else if(rpm < 4000){
        avanceDeChispa = 10;//+-25°
    }else if(rpm < 5000){
        avanceDeChispa = 13;//+-32,5°
    }else if(rpm < 6000){
        avanceDeChispa = 15;//+-37,5°
    }
}
void ControlEncendidoArranque(float temperatura){
    if(temperatura < 45){
        avanceDeChispa = 2;//+-5°
    }else{
        avanceDeChispa = 3;//+-7,5°
    }
}
void iniciarChispazo(int pin){//inicia la chispa hasta que se llame al metodo pararChispazo()
    digitalWrite(pin, HIGH);
}
void pararChispazo(int pin){//para la chispa
    digitalWrite(pin, LOW);//edit FDSoftawre, tenes que mandarle LOW vo' XDD
}
//------------
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
void LCD(int op, int texto){
    String linea1 = "###OpenEFI v0.010###";
    String lin2  = "#RPM: ";
    String lin2a = " #T:";
    String lin2b = "#";
    String lin3a = "###T.INY:";
    String lin3b = " mS####";
    String lin4a = "###Avance: ";
    String lin4b = "°######";

    if(op == 0){
        String LX = lin2 + texto + lin2a + temp + lin2b;
        lcd.setCursor(0,1);
        lcd.print(LX);
    }
    if(op == 1){
        String LX = lin3a + texto + lin3b;
        lcd.setCursor(0,1);
        lcd.print(LX);
    }
    if(op == 2){
        String LX = lin4a + texto + lin4b;
        lcd.setCursor(0,1);
        lcd.print(LX);
    }
    if(op == 5){ //esta opcion es para inciar el panel
        lcd.setCursor(0,0);
        lcd.print(" ###OpenEFI v0.010###");
        lcd.setCursor(0,1);
		lcd.print("#RPM:  #T:°C#");
        lcd.setCursor(0,2);
		lcd.print("###T.INY: mS####");
        lcd.setCursor(0,3);
		lcd.print("###Avance: °######");
    }
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