//
// CD4051.h libreria para manejar MUX analogico en la entrada A0
//

#include "CD4051.h"

#if defined(ESP8266)
	#define PinA FUNC_GPIO15
	#define PinB FUNC_GPIO13
	#define PinC FUNC_GPIO12
#endif
#if defined(__AVR_ATmega328P__)
	#define PinA PORTC3
	#define PinB PORTC2
	#define PinC PORTC1
#endif
CD4051::CD4051(byte op){
	//PORTC pines: 3 = A, 2 = B, 1 = C
#if defined(__AVR_ATmega328P__)
	DDRC |= (1 << PinA);
	DDRC |= (1 << PinB);
	DDRC |= (1 << PinC);
#endif

#if defined(ESP8266)
	pinMode(A0, INPUT);
	pinMode(15, OUTPUT);
	pinMode(13, OUTPUT);
	pinMode(12, OUTPUT);
#endif
}

int CD4051::GetVal(byte in) {
#if defined(__AVR_ATmega328P__)
switch (in){
	case 0:
		PORTC |= (0 << PinA);
		PORTC |= (0 << PinB);
		PORTC |= (0 << PinC);
		return analogRead(A0);
		break;
	case 1:
		PORTC |= (1 << PinA);
		PORTC |= (0 << PinB);
		PORTC |= (0 << PinC);
		return analogRead(A0);
		break;
	case 2 :
		PORTC |= (0 << PinA);
		PORTC |= (1 << PinB);
		PORTC |= (0 << PinC);
		return analogRead(A0);
		break;
	case 3:
		PORTC |= (1 << PinA);
		PORTC |= (1 << PinB);
		PORTC |= (0 << PinC);
		return analogRead(A0);
	case 4:
		PORTC |= (0 << PinA);
		PORTC |= (0 << PinB);
		PORTC |= (1 << PinC);
		return analogRead(A0);
		break;
	case 5:
		PORTC |= (1 << PinA);
		PORTC |= (0 << PinB);
		PORTC |= (1 << PinC);
		return analogRead(A0);
		break;
	case 6:
		PORTC |= (0 << PinA);
		PORTC |= (1 << PinB);
		PORTC |= (1 << PinC);
		return analogRead(A0);
		break;
	case 7:
		PORTC |= (1 << PinA);
		PORTC |= (1 << PinB);
		PORTC |= (1 << PinC);
		return analogRead(A0);
		break;
	default:
		return 0;
		break;
	}
#endif

#if defined(ESP8266)
switch (in){
	case 0:
		GPOC = 1 << PinA;
		GPOC = 1 << PinB;
		GPOC = 1 << PinC;
		break;
	case 1:
		GPOS = 1 << PinA;
		GPOC = 1 << PinB;
		GPOC = 1 << PinC;
		break;
	case 2:
		GPOC = 1 << PinA;
		GPOS = 1 << PinB;
		GPOC = 1 << PinC;
		break;
	case 3:
		GPOS = 1 << PinA;
		GPOS = 1 << PinB;
		GPOC = 1 << PinC;
		break;
	case 4:
		GPOC = 1 << PinA;
		GPOC = 1 << PinB;
		GPOS = 1 << PinC;
		break;
	case 5:
		GPOS = 1 << PinA;
		GPOC = 1 << PinB;
		GPOS = 1 << PinC;
		break;
	case 6:
		GPOC = 1 << PinA;
		GPOS = 1 << PinB;
		GPOS = 1 << PinC;
		break;
	case 7:
		GPOS = 1 << PinA;
		GPOS = 1 << PinB;
		GPOS = 1 << PinC;
		break;
	}
	GPOS = 1 << FUNC_GPIO15;//Registro GPOS enciende salida GPIO
	GPOC = 1 << FUNC_GPIO11;//Registro GPOC apaga salida GPIO
#endif
}