/*
  Timer.h liberia simple para manejo temporizadores para arduino
  by FDSoftware github.com/FDSoftware
  Released into the public domain.
*/

#include "Arduino.h"
#include "Timer.h"

Timer::Timer(){
}

void Timer::init(unsigned long T){
  //void para iniciar nuevo temporizado
  _T = T;
  T2 = micros();
}

bool Timer::cond(){
  //comprueba si se paso el tiempo de temporizado
  //devuelve true o falso si se cumple la condicion
  T_AC = micros();
  if(T_AC - T2 >= _T){
    return true;
  } else return false;
}