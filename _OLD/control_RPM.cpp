void ControlRPM(){  
int dntA   = 0; //numero de diente anterior 
int indice = 0; //indice para guardar en array de Tdnt
int rpmtT  = 0; //variable temporal para calcular rpm

  // codigo para sincronizar el cigueñal
      if(varr == false){
         milirpm = millis();
         varr = true;
		 dntA = vuelta;
     }
  if(indice >= dnt){
     if(varr == true && dntA != vuelta){ //si se detecto un diente nuevo y se guardo el tiempo anterior 
      Tdnt[indice] = millis() - milirpm;
      varr = false;
	  indice++;
     }
  }else{
	sincronizar();
	indice = 0;
  }
  //calculamos las rpm promediando el tiempo de 4 dientes
  //#####CAMBIAAAAAR LUEGOOOOO######
  //sacar promedio de 1/2 del cigueñal
  int rpmT;
  if (indice <= 3){
	rpmT = (Tdnt[0] + Tdnt[1] + Tdnt[2] + Tdnt[3]) / 4; //sacamos el tiempo promedio de los 4
	rpmT = 1000 / rpmT; //divimos el tiempo por 1000 para tener la cantidad de dientes por segundo
	rpmT = dnt /rpmT;   //ahora tenemos la cantidad de vueltas por segundo
	rpm = rpmT * 60;    //finalmente tenemos las RPM
  }
}
int RPM(){ //Proximamente esta funcion manda las rpm promedio de 4-5 vueltas
    return rpm;
}