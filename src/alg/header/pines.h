#pragma once
/*-----( OpenEFI V1.5X )-----*/
/*-----( PINOUT: (Sensores) )-----*/
#define maraccMode 1
//modo de mariposa de aceleracion, en modo (1) utiliza solo el potenciometro 1
//en modo 0 utiliza los dos

#define uEFI 0

#if defined(ESP8266)
//OpenEFI con NodeMCU
byte A = 3, //pines del MUX analogico
	B = 4,
	C = 5;
byte TPS1 = 2;  //Pin de potenciometro de TPS 1
byte TPS2 = 3;  //Pin de potenciometro de TPS 2
byte TEMP2 = 4;  //pin de sensor de temperatura de agua
byte mar = A0; //pin de mariposa de acelerador
byte LMBA = A0; //pin sonda lambda A
byte LMBB = A0; //pin sonda lambda B
byte vmtP = 1;  //pin de ventialador
#endif // OpenEFImode == 1

#if uEFI == 0
#if defined(__AVR_ATmega328P__)
//OpenEFI con Arduino NANO v3
byte A = A3, //pines del MUX analogico
	 B = A2,
	 C = A1;
byte TPS1  = 2;  //Pin de potenciometro de TPS 1
byte TPS2  = 3;  //Pin de potenciometro de TPS 2
byte TEMP2 = 4;  //pin de sensor de temperatura de agua
byte mar   = A1; //pin de mariposa de acelerador
byte LMBA  = A4; //pin sonda lambda A
byte LMBB  = A5; //pin sonda lambda B
byte vmtP  = 4;  //pin de ventialador
#endif // OpenEFImode == 0
#endif

#if uEFI == 1
byte TPS1 = 2;  //Pin de potenciometro de TPS 1
byte TPS2 = 3;  //Pin de potenciometro de TPS 2
byte TEMP2 = 4;  //pin de sensor de temperatura de agua
byte mar = A0; //pin de mariposa de acelerador
byte LMBA = A1; //pin sonda lambda A
byte vmtP = 4;  //pin de ventialador
#endif // uEFI == 1