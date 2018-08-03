void sincronizar(){

    #if(dev == 1) 
        if (sincronizado == false){
            Serial.println("DBG Sincronizando PMS con inyeccion y encendido");
        }
   #endif
    //Esta funcion sincroniza el valor de "vuelta" con el PMS del piston 1
    int TMIN = 99999; //almacena el tiempo menor de cada diente
    int IMIN = 0; //almacena el indice del menor tiempo(al final no la tuve que usar :P)
    int TMAX = 0; //almacena el tiempo mayor (Este seria teoricamente el PMS del piston 1)
    int IMAX = 0; //almacena el indice del diente con PMS del piston 1, luego se restablece la variable diente...
	//a 0 y se pone la variable sincronizado en true para habilitar el resto del programa.

	int DientesEntreHUECOyPMS = 25;//esta variable la cambiamos cuando sepamos cuantos dientes son
	int TamanoDelArray = (sizeof(Tdnt)/sizeof(Tdnt[0]));//Tama�o del array (cantidad de dientes)

	for(int i = 0; i < TamanoDelArray; i++){
			if(Tdnt[i] < TMIN){
				TMIN = Tdnt[i];
			}else if(TMAX < Tdnt[i]){
				TMAX = Tdnt[i];

				IMAX = i + DientesEntreHuecoYPMS;//PMS del piston uno seria...
				//el indice del hueco(tiempo mayor) mas la distancia entre el hueco...
				//y el pms del piston uno
			}
		}
	diente = TamanoDelArray-IMAX;//el diente actual es la distancia que tenemos..
	//desde el diente 0 que es el diente del pms del piston 1(IMAX)


	//for para obtener el promedio de tiempo entre dientes y modificarlo en la clase variables.h
	int aux = 0;
	for(int i = ; i < TamanoDelArray; i++){
		aux+=Tdnt[i];
	}
	promedio = aux/TamanoDelArray;//fianlmente cambiamos el promedio
	//una vez ejecutada esta funcion se puede usar con seguridad la variable dientes y el promedio :P
    vuelta = 0;
    sincronizado = true;
}
