//
// CD4051.h libreria para manejar MUX analogico en la entrada A0
//

#include "CD4051.h"

#if defined(__AVR_ATmega328P__)
	#define PinA PORTC3
	#define PinB PORTC2
	#define PinC PORTC1
#endif
#if defined(BLUEPILL_F103C6) //para ecu con stm32f103
//se supone que al manejar todo el puerto de una patada va a ser mas rapido que
// el arduino uno
	//Set A2, A12, A13 (HIGH)
	GPIOA->regs->ODR |= 0b0011000000000100;
	//Clear A2, A12, A13 (LOW)
	GPIOA->regs->ODR &= ~(0b0011000000000100);
#endif
CD4051::CD4051(byte op){
	//PORTC pines: 3 = A, 2 = B, 1 = C
#if defined(__AVR_ATmega328P__)
	DDRC |= (1 << PinA);
	DDRC |= (1 << PinB);
	DDRC |= (1 << PinC);
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
#if defined(BLUEPILL_F103C6)
switch (in) {
	case 0:
		GPIOA->regs->ODR &= ~(0b0011000000000100);
		return analogRead(A0);
		break;
	case 1:
		PORTC |= (1 << PinA);
		PORTC |= (0 << PinB);
		PORTC |= (0 << PinC);
		return analogRead(A0);
		break;
	case 2:
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
}