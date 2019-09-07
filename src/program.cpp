/* Para no hacer desastre, el programa principal de OpenEFI va a aca */
#include <Arduino.h>
#include <Wire.h>

#include "defines.h"
#include "variables.h"
#include "C_PWM.h"

#include "interfazSerial.h"
#include "interfazCAN.h"

interfazSerial Ser;
//pines pa probar:
uint8_t pinesE[] = {3,4,5,6};
uint8_t pinesI[] = {6,7,8,9};

C_PWM MyPWM(pinesE , pinesI);

void program_setup (void){
    Ser = interfazSerial();
    Ser.send(F("MSG"), F("Starting up"));
    Ser.send(F("VER"), 200);
    // TODO
}

void program_loop (void){

}

void RPM(){ //void boludo para calcular las rpm, recuperado de las profundidades del git
  T_RPM_AC = millis();
  if (T_RPM_AC - T_RPM_A >= RPM_per) {
        T_RPM_A = T_RPM_AC;
        _RPM = (_POS / DNT)*120; //calculo para obtener las rpm
        _POS = 0;
        Ser.rpm();
    }
}

#if Alpha == 1
//interrupcion para rpm (SIN SINCRONIZAR PMS NO USAR EN UN MOTOR O ROMPE' TODO)
void I_RPM() { 
    _POS++;
    MyPWM.Intr();
    MyPWM.Iny();
    MyPWM.Ecn();
}
#else
void I_RPM() { //interrupcion para rpm
    if(SINC){
        _POS++;
        MyPWM.Intr();
    }else sincINT();
}

void sincINT() {
    //interruppcion para "sincronizar()"
    if(!initsinc){
        Ta = micros();
        initsinc = true;
    }else{
        if(sincB){
            Tb = micros();
            T1 = Tb - Ta;
            Ta = Tb;
            sincB = false;
        }else{
            Tb = micros();
            T2 = Tb - Ta;
            Ta = Tb;
            sincB = true;
            sincronizar();
        }
    }
}
void sincronizar() {
    //este void caza el ultimo tiempo entre diente y si es mayor por 1.5 veces al anterior,
    //marca que es el PMS
    if( T2 > ( T1 + (T1 / 2.3 ))){
        SINC = true;
    }
}
#endif