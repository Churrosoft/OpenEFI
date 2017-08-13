char LeerLinea(int pos,String nombre){ //posicion, Nombre de archivo

	File IN = SD.open(nombre); //abrimos el archivo
	IN.seek(pos); //ponemos el cursor en la linea 5
	char X;
	do{
		X = IN.peek(); //leemos byte de la linea seleccionada con seek, devuelve -1 si no hay mas datos
		if(X != "-1"){
			linea[i] = X;
			i++;
		}
	}while(X != "-1");
	
	IN.close();//Ceramos el archivo
	return linea;
}