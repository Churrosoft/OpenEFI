#if (motor == 1)
/*-----( Variable Globales )-----*/
//estas variables las puede usar cualquieeer funcion
int _RPM       = 0; //las rpm :V
int _TEMP      = 0; //temperatura
int _POS       = 0; //posicion del cigueñal (en dientes)
int _AE        = 0; //avance de encendido

int AVC_E(float temperatura){

    //si las rpm son menores a 600...
    if(_RPM < 600){
		//controlador de avance en frio
        return avanceEnArranque();
		//si son mayores a 600
    }else if(_RPM >=600){
		//controlador de avance normal (multi temperatura y rmp)
		return avanceEnNormal(temperatura);

    }

//--------------------BLOQUE DE CONTROLADORES DE AVANCE--------------------------

void avanceEnArranque(){

	int avanceAnterior = avanceDeChispa;
	int avc2 = dientes(3);
	return avc2;

	if(avanceAnterior != avanceDeChispa){

		LCD(2,avanceDeChispa,0);

	}
}
int avanceEnNormal(float temperatura){

	int indiceTemp = 0;//indice que indica a que columna de la matriz acceder(temperatura)
	int indiceRPM = 0;//indice que indica a que fila de la matriz acceder(RPM)

	//si la temperatura es menor a 98 buscamos el indice adecuado
    if(temperatura < 98){

		indiceTemp = ((temperatura < 18)? 0:((temperatura < 27)? 1:((temperatura < 36)? 2:((temperatura < 45)? 3:((temperatura < 55)? 4:((temperatura < 64)? 5:((temperatura < 73)? 6:((temperatura < 82)? 7:((temperatura < 91)? 8:9)))))))));
		//operaciones ternarias para asignar el indice a leer en tabla segun la temperatura
		//este indice corresponde a las columnas (eje X)

	//y si la temp no es menor a 98 elegimos el indice mayor en la tabla(a full papa!).
	}else{

		indiceTemp = 10;

	}
	indiceRPM = ((_RPM<1000)?0:((_RPM<1200)?1:((_RPM<1500)?2:((_RPM<1700)?3:((_RPM<2000)?4:((_RPM<2200)?5:((_RPM<2500)?6:((_RPM<2700)?7:((_RPM<3000)?8:((_RPM<3200)?9:((_RPM<3500)?10:((_RPM<3700)?11:((_RPM<4000)?12:((_RPM<4500)?13:((_RPM<5000)?14:((_RPM<5500)?15:((_RPM<6000)?16:17))))))))))))))))); 
	//operaciones ternarias para asignar el indice a leer en tabla segun las RPM
	//este indice corresponde a las filas (eje Y)
	avanceAnterior = avanceDeChispa;
	//finalmente accedemos al valor en tabla correspondiente al estado actual del motor
	int avc2 = dientes(tablaAvance[indiceRPM][indiceTemp]);
	//y lo retornamos
	return avc2;
	
}
//--------------------FIN DE BLOQUE DE CONTROLADORES DE AVANCE----------------

#endif