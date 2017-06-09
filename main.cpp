//Programa principal
//Creado por FDSoftware y FrancisJPK

#include <Arduino.h>

int ord_enc[] = {0, 2 ,3 ,1};//orden de encendido motor
int iny[] = {2,3,4,5};       //Pines de arduino que esta conectados los inyectores
int tMax     = 22;           //tiempo maximo de inyeccion

int pinBobinas13 = 1;
int pinBobinas24 = 2;

int pul  = LOW;             //pulso de disparo de sensor de posicion de cigueñal
bool varrpm = LOW;          //variable para no volver a contar mismo diente como pulso
int pinrpm = 10;            //pin conectado al sensor de posicion de cigueñal

int diente = 0;             //numero de diente actual de la corona
int intervalo1 = 500;       //intervalo para medir rpm
float inyT = 1.5;              //tiempo de inyeccion ms
float inyTARR = 22;        //tiempo inyeccion en modo arranque
int esperaINY = 500;      //tiempo que espera inyeccion para ver si el motor acelera


int avanceDeChispa = 3;        //en dientes de volante (2,42 grados) , default en 3
int dnt = 20;                  //cantidad de dientes que detecta el sensor hall
int perinyec = 5;               //cantidad de dientes que tiene que detectar el sensor hall para inyectar

int mar   = 0;            //pin de mariposa de acelerador
int marv  = 0;              //valor de mariposa de acelerador
int Lamb  = 1;            //pin de sonda lambda
int sensorT = 3;           //pin de sensor Temperatura
int ARRpin = 10;            //pin conectado que recibe señal de arranque de llave
bool arr = false;           //si se intenta arrancar el motor esta en true

float temp = 0;             //temperatura motor
float tempfrio = 40;        //temperatura maxima para considerar el motor "frio"
float tempvenMIN = 80;      //temperatura a la que se apaga el ventilador
float tempvenMAX = 95;      //temperatura a la que se enciende el ventilador
float tempmax = 115;        //temperatura maxima de funcionamiento antes de entrar en modo emergencia

int pinVENT = 13;           //pin de control del ventilador
int pinLuzMuerte = 12;       //sip, pin de luz de "check Engine"
bool emergencia = false;    //mientras que este en false todo bien ^~^

int var = 0;                //variable usada para bucles
int varX = 0;               //variable temporal, multiples usos
int varINY = 0;              //varible temporal, guarda ultimo inyector activado

int rpm    = 0;                //numero actual de rpm
int rpmMIN = 800;             //numero minimo de rpm
int rpmMAX = 7000;            //numero maximo de rpm
int tolrpm = 50;              //tolerancia de rpm
int rpmant = 0;

int vuelta = 0;                    //numero de vuelta para rpm
bool varr = false;             //variable para evitar calcular rpm varias veces
int vuelta2 = 0;                   //numero de vuelta para inyeccion/encendido
int vuelta3 = 0;
bool inyectando = false;            //variable para saber si se esta inyectando todavia o no
bool acelerar;                 //varible temporar, guarda si se intento acelerar las rpm o no
unsigned long prevMillis; //tiempo previo ultimo periodo medicion rpm
unsigned long curMillis; //tiempo actual del micro
unsigned long milisant;
unsigned long tmpINY = 0; //tiempo que va inyectando
bool arrancando = false;
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

    for(var = 0; var >= 3; var++){
        pinMode(iny[var], OUTPUT);
    }

    for(var = 0; var >= 3; var++){
        digitalWrite(iny[var], LOW);
    }
    //definir interrupciones HW
     attachInterrupt(digitalPinToInterrupt(2), int0, RISING);
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
    milisant = millis();
    varr = true;
     }
  
     if(varr == true && vuelta == 50){
      rpm = ((millis() - milisant)* 4 )* 60000;
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

    int periodo = 7; //periodo en mS
    unsigned long tiempo  = 0;
    unsigned long millisant = 0;
    bool activado = false;

        if(diente == (33 - avanceDeChispa) ){//----chispazo para el piston 1 y 3(siendo el 3 chispa perdida)
          iniciarChispazo(pinBobinas13);//iniciar chispazo
            activado = true;
            //esperar un tiempito
            if(activado == true){
                tiempo = millis();
            }
            if ((millis() - millisant) >= periodo && activado == true) {
                pararChispazo(pinBobinas13);//una vez pasados 5ms terminar chispazo
                milisant = millis();
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
                milisant = millis();
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
                milisant = millis();
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
              //  pararChispazo(pinBobinas24);
                milisant = millis();
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
        ControlEncendidoArranque(temp);
        arrancando = true;
        arr = true;
        inyT = inyTARR;
        return false;
    }if (senalarr == LOW && rpm >= rpmMIN) {
        return true; //habilitamos el control de inyeccion 
        arrancando = false;
    }
}


float  sensortemp(){
    int value = analogRead(sensorT);
    float millivolts = (value / 1023.0) * 5000;
    float celsius = millivolts / 10; 
    return celsius;
} 





void emerg(){
    digitalWrite(pinLuzMuerte, HIGH);
    int varemerg = 1;
    while(varemerg == 1){
        delay(1000);
    }
}