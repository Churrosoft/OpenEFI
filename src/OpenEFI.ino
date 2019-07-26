#include <Arduino.h>
#include <Wire.h>

#include "defines.h"
#include "PWM.h"

#include "interfazSerial.h"
#include "interfazCAN.h"


// Inicio de librerias
interfazSerial Ser;

void setup(){
    Ser = interfazSerial();
    Ser.send(F("MSG"), F("Starting up"));
    Ser.send(F("VER"), 200);
    // TODO
}

void loop(){
    // TODO
    Ser.query();
}