/*
  Timer.h liberia simple para manejo temporizadores para arduino
  by FDSoftware github.com/FDSoftware
  Released into the public domain. 
*/

#ifndef Timer_h
#define Timer_h

#include "Arduino.h"

class Timer{
  public:
    Timer(unsigned long t);
    void init();
    bool TL();
  private:
    unsigned long _T;
    unsigned long T2;
    unsigned long T_AC;
};

#endif