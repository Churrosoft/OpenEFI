#if (motor == 1)
//------------------------(FrancisJPK inicio)-----------------------------------------

/*-----( Variable Globales )-----*/
//estas variables las puede usar cualquieeer funcion
int _RPM       = 0; //las rpm :V
int _TEMP      = 0; //temperatura
int _POS       = 0; //posicion del cigueñal (en dientes)
int _AE        = 0; //avance de encendido

int AVC_E(float temperatura){

    //----CONTROL DE AVANCE ANTES DE MANDAR CHISPA----
    if(_RPM < 600){
		//controlador de avance en frio
        return avanceArranque();
    }else if(_RPM >=600){
		return avanceNormal(temperatura);
    }
    //---------------FIN DE CONTROL DE AVANCE PARA PROCEDER A MANDAR CHISPA-----------
	//--------------------------------------------------------------------------------
    //borrado control de mandada de chispa
//------------------------------------------------------------------------------
//--------------------BLOQUE DE CONTROLADORES DE AVANCE--------------------------

void avanceArranque(){
	//avanceAnterior = avanceDeChispa; FDSoftware: da error despue fijate XD
	int avc2 = dientes(3);
	return avc2;
	//if(avanceAnterior != avanceDeChispa){
	//	LCD(2,avanceDeChispa,0);
	//}
}
int avanceNormal(float temperatura){

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
	int avc2 = dientes(tablaAvance[indiceRPM][indiceTemp]);
	return avc2;
	//finalmente accedemos al valor en tabla correspondiente al estado actual del motor
	//-----------FIN DEL BLOQUE DE ASIGNACION DE INDICES A LEER EN TABLAS-----
}
//--------------------FIN DE BLOQUE DE CONTROLADORES DE AVANCE----------------
//-----------------------------------------------------------------------------
//---------------------FUNCIONES DE CONTROL DE BOBINAS------------------------

#endif
//---------------------(FrancisJPK final)---------------------------------------------------