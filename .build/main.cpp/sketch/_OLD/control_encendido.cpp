#if (motor == 1)
//------------------------(FrancisJPK inicio)-----------------------------------------
void controlDeEncendido(float temperatura){

    //----CONTROL DE AVANCE ANTES DE MANDAR CHISPA----
    if(arrancando == true){
		//controlador de avance en frio
        avanceArranque();
    }else if(arrancando == false){
		avanceNormal(temperatura);
    }
    //---------------FIN DE CONTROL DE AVANCE PARA PROCEDER A MANDAR CHISPA-----------
	//--------------------------------------------------------------------------------
    //----------------------------------CONTROL DE MANDADA DE CHISPA-----------------
        if(diente >= ((dnt/4) - avanceDeChispa) && diente <= ((dnt/4)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas14);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 1 y 4(siendo el 4 chispa perdida)
        }else if(diente >= ((dnt/2) - avanceDeChispa) && diente <= ((dnt/2)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas23);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 2 y 3(siendo el 2 chispa perdida)
        }else if(diente >= (((dnt/4)*3) - avanceDeChispa) && diente <= (((dnt/4)*3)-avanceDeChispa)+1){
            iniciarChispazo(pinBobinas14);
            activado = true;
            millisant = millis();
            //----chispazo para el piston 1 y 4(siendo el 1 chispa perdida)
        }else if(diente >= (dnt - avanceDeChispa) && diente <= (dnt - avanceDeChispa)+1 ){
            iniciarChispazo(pinBobinas23);
            millisant = millis();
            activado = true;
            //----chispazo para el piston 2 y 3(siendo el 3 chispa perdida)
        }

        if ((millis() - millisant) >= periodo && activado == true) {
            millisant = millis();
            activado = false;
            pararChispazo(pinBobinas14);
            pararChispazo(pinBobinas23);
        }
}//------------------FIN DE CONTROL DE MANDADA DE CHISPA-------------------------
//------------------------------------------------------------------------------
//--------------------BLOQUE DE CONTROLADORES DE AVANCE--------------------------

void avanceArranque(){
	//avanceAnterior = avanceDeChispa; FDSoftware: da error despue fijate XD
	avanceDeChispa = dientes(3);
	//if(avanceAnterior != avanceDeChispa){
	//	LCD(2,avanceDeChispa,0);
	//}
}
void avanceNormal(float temperatura){

	int indiceTemp = 0;//indice que indica a que columna de la matriz acceder(temperatura)
	int indiceRPM = 0;//indice que indica a que fila de la matriz acceder(RPM)

	//------------BLOQUE DE ASIGNACION DE INDICES A LEER EN TABLAS------------
	//if{temperatura < 98} //FDSoftware: weeeey los if van con () no con {}
    if(temp < 98){
		indiceTemp = ((temperatura < 18)? 0:((temperatura < 27)? 1:((temperatura < 36)? 2:((temperatura < 45)? 3:((temperatura < 55)? 4:((temperatura < 64)? 5:((temperatura < 73)? 6:((temperatura < 82)? 7:((temperatura < 91)? 8:9)))))))));
		//este bodoque gigante de operaciones ternarias es para asignar el indice a leer en tabla segun la temperatura
		//este indice corresponde a las columnas (eje X)
	}else{
		indiceTemp = 10;
	}
	indiceRPM = ((RPM<1000)?0:((RPM<1200)?1:((RPM<1500)?2:((RPM<1700)?3:((RPM<2000)?4:((RPM<2200)?5:((RPM<2500)?6:((RPM<2700)?7:((RPM<3000)?8:((RPM<3200)?9:((RPM<3500)?10:((RPM<3700)?11:((RPM<4000)?12:((RPM<4500)?13:((RPM<5000)?14:((RPM<5500)?15:((RPM<6000)?16:17))))))))))))))))); 
	//este otro bodoque gigante de operaciones ternarias es para asignar el indice a leer en tabla segun las RPM
	//este indice corresponde a las filas (eje Y)
	//---------
	//avanceAnterior = avanceDeChispa; FDSoftware: RT :V, marca error de que no la declaraste :V
	avanceDeChispa = dientes(tablaAvance[indiceRPM][indiceTemp]);
	//finalmente accedemos al valor en tabla correspondiente al estado actual del motor
	//if(avanceAnterior != avanceDeChispa){ RT :,v
	//	LCD(2,avanceDeChispa,0);
	//}
	//-----------FIN DEL BLOQUE DE ASIGNACION DE INDICES A LEER EN TABLAS-----
}
//--------------------FIN DE BLOQUE DE CONTROLADORES DE AVANCE----------------
//-----------------------------------------------------------------------------
//---------------------FUNCIONES DE CONTROL DE BOBINAS------------------------

#endif
void iniciarChispazo(int pin){//inicia la chispa hasta que se llame al metodo pararChispazo()
    digitalWrite(pin, HIGH);
}
void pararChispazo(int pin){//para la chispa
    digitalWrite(pin, LOW);
}
//---------------------(FrancisJPK final)---------------------------------------------------