// 
// CD4051.h libreria para manejar MUX analogico en la entrada A0
// 

#include "CD4051.h"

CD4051::CD4051(byte op){
	//PORTC pines: 3 = A, 2 = B, 1 = C
#if defined(__AVR_ATmega328P__)
	DDRC |= (1 << PORTC3);
	DDRC |= (1 << PORTC2);
	DDRC |= (1 << PORTC1);
#endif

#if defined(ESP8266)
#endif
}

int CD4051::GetVal(byte in) {
#if defined(__AVR_ATmega328P__)

	switch (in){
	case 0:
		PORTC |= (0 << PORTC3);
		PORTC |= (0 << PORTC2);
		PORTC |= (0 << PORTC1);
		return analogRead(A0);
		break;
	case 1:
		PORTC |= (1 << PORTC3);
		PORTC |= (0 << PORTC2);
		PORTC |= (0 << PORTC1);
		return analogRead(A0);
		break;
	case 2 :
		PORTC |= (0 << PORTC3);
		PORTC |= (1 << PORTC2);
		PORTC |= (0 << PORTC1);
		return analogRead(A0);
		break;
	case 3:
		PORTC |= (1 << PORTC3);
		PORTC |= (1 << PORTC2);
		PORTC |= (0 << PORTC1);
		return analogRead(A0);
	case 4:
		PORTC |= (0 << PORTC3);
		PORTC |= (0 << PORTC2);
		PORTC |= (1 << PORTC1);
		return analogRead(A0);
		break;
	case 5:
		PORTC |= (1 << PORTC3);
		PORTC |= (0 << PORTC2);
		PORTC |= (1 << PORTC1);
		return analogRead(A0);
		break;
	case 6:
		PORTC |= (0 << PORTC3);
		PORTC |= (1 << PORTC2);
		PORTC |= (1 << PORTC1);
		return analogRead(A0);
		break;
	case 7:
		PORTC |= (1 << PORTC3);
		PORTC |= (1 << PORTC2);
		PORTC |= (1 << PORTC1);
		return analogRead(A0);
		break;
	default:
		return 0;
		break;
	}
#endif

#if defined(ESP8266)
	GPOS = 1 << FUNC_GPIO12;//Registro GPOS enciende salida GPIO
	GPOC = 1 << FUNC_GPIO11;//Registro GPOC apaga salida GPIO
#endif
}