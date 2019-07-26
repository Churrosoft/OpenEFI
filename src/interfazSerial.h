#pragma once
#ifndef _INTERFAZSERIAL_h
#define _INTERFAZSERIAL_h
#include <Arduino.h>

/*
Formato de mensajes:
  String comando + " " + String mensaje + \n

///Comandos (salida)///
   - MSG: Mensajes de estado
   - ERR: Mensajes de error
   - DBG: para espamear el serial
   - RPM X: RPM actuales del motor
   - TMP X: Temperatura de refrigerante
   - BAT X: Voltaje batería
   - VER X: Version de OpenEFI
   - ACK <mensaje>: Confirma recepcion de mensaje
   - NAK <mensaje>: Indica que el mensaje no ha sido procesado
///Comandos (entrada)///
   - <RPM/TMP/BAT> [ON/OFF]: Activa/desactiva la transmisión de información. 
                            Sin parámetros solo envía el valor actual

 */
class interfazSerial{
    public:
    interfazSerial();
    void send(String command, String message);
    void send(String command, int message);

    void query();
    private:
    char inputBuffer[32];
    void processInput(char* input);
};

#endif