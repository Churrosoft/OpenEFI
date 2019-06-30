#pragma once
#ifndef _INTERFAZSERIAL_h
#define _INTERFAZSERIAL_h
#include "Arduino.h"
/*
//aca voy a definir temporalmente el protocolo por serie que usa OpenEFI
#########################################################################

-----( Comandos de estado )-----
info:
estos comandos los manda la ecu para informar el estado del sistema
se envian automaticamente
TMP  X : Temperatura de refrigerante
RPM  X : RPM actuales del motor
00V  X : Voltaje de bateria
AVC  X : Avance del motor
INY  X : Tiempo de inyeccion
LMB  X : Voltaje de sonda lambda
DBG  X : Mensaje de debug
viendo que tengo que agregar comandos para edicion de tablas de inyeccion,
codigos de error DTC y demas, voy a cambiar el formato de los mensajes a algo asi:
(Identificador mensaje(3 numeros) )(separador " ; " )(Sub identificador (3 letras))(separador " ; " )(Mensaje (hasta 25 caracteres) )

-----( Identificador de mensaje )-----

000 = Estado Sistema
001 = Estado inyeccion y avance
002 = Lectura de datos a memoria
003 = escritura de datos a la memoria
004 = debug (aca tambien entraria FixedMode)
005 = codigos de falla DTC
006 = pruebas motor (balanceo potencia, etc)
999 = ACK de la ecu, recien cuando llega esto se puede enviar otro mensaje

-----( Sub identificadores: 000 )-----

TMP  X : Temperatura de refrigerante
RPM  X : RPM actuales del motor
00V  X : Voltaje de bateria

-----( Sub identificadores: 001 )-----

AVC  X : Avance del motor
INY  X : Tiempo de inyeccion
LMB  X : Voltaje de sonda lambda

-----( Sub identificadores: 002 )-----

-----( Sub identificadores: 003 )-----

-----( Sub identificadores: 004 )-----

FXM_A X : cambio de avance del modo fijo
FXM_I X : cambio de tiempo de inyeccion del modo fijo
FXM_E X : habilita el FixedMode
FXM_D X : deshabilita el FixedMode

-----( Ejemplo mensaje:)-----
000 ; RPM ; 2500
*/
class interfazSerial{

public:
	interfazSerial(byte d);
	void Enviar(byte code, String SubCode, String msg);
	void Enviar(byte code, String SubCode, int msg);
	int Getcode();
	String GetSubCode();
	String GetMSG();
	int GetMSG_int();
	void loop();
	bool MSG();// Devuevle true si hay un mensaje nuevo
	void MSG(bool op); //sobrecarga funcion ,la uso para informar que ya use el ultimo mensaje
	bool FXMD();
	int FXMD(bool mode, int val); //obtiene el tiempo y avance nuevo para el FixedMode
private:
	String msg;
	bool _msg = false;
	String temp;
};

#endif