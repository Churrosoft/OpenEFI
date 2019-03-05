//Funcion de arranque en frio
//hasta que el motor no alcanze las rpm de regimen no sale de esta Funcion
//pero si pasan 15 vueltas sin poder subir las rpm manda el sistema a modo emergencia

//IMPORTANTEEEEEEEEE
//Agregar los bool y if a los hilos para habiliarlos o no desde cualquier Funcion
int _RPM;
int  INYT1      = 0;
//variables para modificar estado de hilos
bool A_H1 = true;
bool A_H2 = true;
bool A_H3 = true;
//variables ARR_F
long ARR_BAL = 450000; //"Baldazo" de nafta/diesel para acelerar el motor (uS)
int  ARR_BP  = 34;	   //cantidad de vueltas que dura el baldazo
long ARR_DI  = 1200;   //tiempo de inyeccio que se resta luego del baldazo (uS)
long ARR_RT  = 21000;  //tiempo de inyeccion base en ralenti (uS)
int  ARR_PB  = 5;	   //cantidad de vueltas a esperar antes de disminuir la cantidad de nafta luego del "baldazo"
int  ARR_IN  = 12;	   //cantidad de vueltas a esperar para ver si suben las rpm, caso contrario se considera fallido el arranque
int  ARR_V   = 0;	   //contador de vueltas de cigueñal
int  ARR_F1  = 500;	   //Flag 1 cantidad de vueltas minimas para salir del "baldazo"

void ARR_F(){
	A_H2 = false;
	A_H3 = false;
	bool _ARR_FIN = false; //en false cuando no se termino de arrancar el motor
	do{
		if(_RPM < ARR_F1){
			do{
				INYT1 = ARR_BAL;
				C_PWM();
			}while(ARR_V > ARR_BP);
			ARR_V = 0;
		}
		int ARR_X = ARR_BAL / 4; // para saber cuanto es el cuarto del "baldazo", cuando el tiempo es menor a eso se pone el tiempo de inyeccion de ralenti
		bool ARR_C  = (INYT1 >= ARR_X);
		bool ARR_C1 = (ARR_V > ARR_BP);
		//agregar luego a los dos if de abajo otros mas para controlar la cantidad de vueltas
		if(ARR_C && ARR_C1){
			//Bloque 1 , las vuelas coinciden y todavia hay que seguir bajando el tiempo de inyeccion
			INYT1 = INYT1 - (ARR_BAL / 8);
			ARR_V = 0;
		}

		if(!ARR_C && ARR_C1){
			//Bloque 2, las vueltas coinciden y no hace falta bajar el tiempo de inyeccion
			ARR_V = 0;
			_ARR_FIN = true;
		}
		C_PWM();
	}while(!_ARR_FIN);
	A_H2 = true;
	A_H3 = true;
}