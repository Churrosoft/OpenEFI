#include "interfazSerial.h"
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
	004 = debug
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

-----( Ejemplo mensaje:)-----
 000 ; RPM ; 2500
*/


interfazSerial::interfazSerial(byte d){
	if(d == 2){
		Serial.begin(9600);
	}
	Serial.begin(11520);
}

void interfazSerial::Enviar(byte code, String SubCode, String msg){
	//este void caza todo, crea el mensaje y lo manda
	temp = "";
	temp += code;
	temp += SubCode;
	temp += msg;
	Serial.println(temp);
}


byte interfazSerial::Getcode(){ 
	temp = msg;
	temp.remove(3,20);
	//devuelvo codigo del ultimo mensaje recibido
	return temp.toInt();
}

String interfazSerial::GetSubCode(){
	//devuelvo subcodigo del ultimo mensaje recibido
	temp = msg;
	temp.remove(0,3);
	temp.remove(4,20);
	return temp;
}

String interfazSerial::GetMSG(){
	//devuelvo la info del ultimo mensaje
	temp.remove(0,5);
	return temp;
}

void interfazSerial::loop(){
	//loop para actualizar mensajes en cola
	while (Serial.available()) {
		char _T = (char)Serial.read();
		msg += _T;
		if (_T == '\n') {
			_msg = true;
		}
	}
}

