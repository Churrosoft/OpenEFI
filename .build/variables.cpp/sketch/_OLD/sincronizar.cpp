void sincronizar(){
    #if(dev == 1) 
        if (sincronizado == false){
            Serial.println("DBG Sincronizando PMS con inyeccion y encendido");
        }
   #endif
    //Esta funcion sincroniza el valor de "vuelta" con el PMS del piston 1
    int TMIN = 99999; //almacena el tiempo menor de cada diente
    int IMIN = 0; //almacena el indice del menor tiempo
    int TMAX = 0; //almacena el tiempo mayor (Este seria teoricamente el PMS del piston 1)
    int IMAX = 0; //almacena el indice del diente con PMS del piston 1, luego se restablece la variable diente...
    //a 0 y se pone la variable sincronizado en true para habilitar el resto del programa.
    int i2   = 0; //variable para recorer array de tiempos
    for(int i = 0; i < dnt; i++){ //i menor a dnt porque empieza la cuenta en 0 el for y los dientes no
        if (Tdnt[i2] < TMIN){
            TMIN = Tdnt[i2];}
        if (Tdnt[i2] > TMAX){
            TMIN = Tdnt[i2];}
        i2++;
    }
    vuelta = 0;
    sincronizado = true;
}
