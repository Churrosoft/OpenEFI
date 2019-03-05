#include "Memory.h"
#include "AT24CX.h"
//Libreria de memoria i2c : https://github.com/cyberp/AT24Cx
AT24C32 mem;
#define col 11
#define fil 18
#define DTCAddress 2500
Memory::Memory(int p){

}

void Memory::Actualizar(int val, int x, int y){
	pos = x * 2 + y * 2; //teoricamente aca tendria que estar el lugar donde voy a guardar
	mem.writeInt(pos,val);
}

int Memory::GetVal(byte tabla, byte x, byte y){
	switch (tabla) {
	case 0: //Avance
		pos = 0;
		break;
	case 1: //Tiempo de inyeccion base
		pos = col * 2 + fil * 2;
		break;
	case 2://Eficiencia Volumetrica (VE)
		pos = (col * 2 + fil * 2) * 2;
		break;
	case 3: //Correccion por temperatura(inyeccion)
		pos = (col * 2 + fil * 2) * 3;
		break;
	case 4: //Correccion por temperatura (encendido) o Correccion por voltaje inyecccion
		pos = (col * 2 + fil * 2) * 4;
		break;
	}
	pos += 11 * y;
	pos += x;
	return mem.readInt(pos);
}

void Memory::Leer(int **tabla, int tipo){
	//voy a guardar todos los datos de la tabla de corrido, asi que, primero grabo datos desde [0][0],
	//hasta [0][11] y luego repito con las demas filas
	switch (tipo){
	case 0: //Avance
		pos = 0;
		break;
	case 1: //Tiempo de inyeccion base
		pos = col * 2 + fil * 2;
		break;
	case 2://Eficiencia Volumetrica (VE)
		pos = (col * 2 + fil * 2) * 2;
		break;
	case 3: //Correccion por temperatura(inyeccion)
		pos = (col * 2 + fil * 2) * 3;
		break;
	case 4: //Correccion por temperatura (encendido) o Correccion por voltaje inyecccion
		pos = (col * 2 + fil * 2) * 4;
		break;
	}
	for (i = 0; i <= fil; i++ ) {
		for (i2 = 0; i2 <= col; i++) {
			tabla[i][i2] = mem.readInt(pos);
			pos += 2;
		}
	}
	pos = 0;
}

int * Memory::GetDTC(){
	pos = DTCAddress;
	int *DTC = malloc(12);
	for (i = 0; i <= 12; i++) {
		DTC[i] = mem.readInt(pos);
		pos += 2;
	}
	return DTC;
}

void Memory::SetDTC(int code){
	//este void guarda un DTC nuevo en la memoria
	bool grabado = false;
	byte posAUX = 0;
	do{
		if( mem.readInt(  DTCAddress + posAUX) == 0 ) {
			mem.writeInt( DTCAddress + posAUX, code );
			grabado = true;
		}
		else { posAUX += 2; }

	}while( grabado != true || posAUX <= DTCAddress + 24 );
}
