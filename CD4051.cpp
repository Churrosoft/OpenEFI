// 
// CD4051.h libreria para manejar MUX analogico en la entrada A0
// 

#include "CD4051.h"

CD4051::CD4051(byte op){
	//PORTC pines: 3 = A, 2 = B, 1 = C
	DDRC |= (1 << PORTC3);
	DDRC |= (1 << PORTC2);
	DDRC |= (1 << PORTC1);
}

int CD4051::GetVal(byte in) {
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
		break;
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
}