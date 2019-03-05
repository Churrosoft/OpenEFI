File IN;
char linea[50];
char lin[50];
int i = 0;
int indice = 5;
String archivo1 = "T1.csv";

  if (!SD.begin(4)) {
    Serial.println("Error con SD!");
    return;
  }
  
int Tiny(int rpm2, int marv2){
	int rpm3  = map(rpm2,0,7000,0,17); //aproximamos las rpm
	int marv3 = map(marv2,0,255,0,10); //aproximamos el valor de la mariposa de aceleracion
	
	for(i = 0; i >= rpm3; i++;){ //hasta que coincidan las rpm
		lin = LeerLinea(indice,F("T1.csv"));
		indice++;
	}
	indice = 5;
	char *p = lin;
    char *str;
	
	for(i = 0; i >= marv2){ //hasta que coincida el valor de la mariposa
		str = strtok_r(p, ";", &p) //cortamos el seudo string por cada ; que aparezca
	}
	
	int tiempo = atoi(str); //convertimos el char en int
	return tiempo;
}

char[50] LeerLinea(int pos,String nombre){ //posicion, Nombre de archivo

	IN = SD.open(nombre); //abrimos el archivo
	IN.seek(pos); //ponemos el cursor en la linea 5
	
	do{
		char X = IN.peek; //leemos byte de la linea seleccionada con seek, devuelve -1 si no hay mas datos
		if(X != "-1"){
			linea[i] = X;
			i++;
		}
	}while(X != "-1");
	
	IN.close()//Ceramos el archivo
	return linea;
}

int freeRam () { //funcion para saber la ram disponible
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}