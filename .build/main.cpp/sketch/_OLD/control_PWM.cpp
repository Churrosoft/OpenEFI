void ControlPWM(){
    unsigned long tmpINY;
    //cambiar esto para hacerlo cada X° del cigueñal o dientes
    if(emergencia == false){ //mientras que no estemos en modo de emergencia V:

        if (inye == false  && per == true){
          digitalWrite(iny[vuelta2++],HIGH);
            tmpINY = micros();
            inye = true;
            per = false; //per es para controlar que se inyecte una sola vez :P
        }
      if (micros() - tmpINY >= inyT) { //Cuando pase el tiempo de inyeccion
                digitalWrite(iny[vuelta2++],LOW); //apagamos el inyector :P
        if(vuelta2 == (cilindros - 1)){ //cuando la cantidad de vueltas sea igual al n° de cilindros
          vuelta2 == 0;
        }
        inye == false;
      }

    }
}