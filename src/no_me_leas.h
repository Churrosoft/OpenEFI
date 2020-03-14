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

00 = Estado Sistema
01 = Estado inyeccion y avance
02 = Lectura de datos a memoria
03 = escritura de datos a la memoria
04 = debug (aca tambien entraria FixedMode)
05 = codigos de falla DTC
00 = pruebas motor (balanceo potencia, etc)
99 = ACK de la ecu, recien cuando llega esto se puede enviar otro mensaje

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