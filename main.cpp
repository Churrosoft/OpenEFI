//Programa principal
//Creado por FDSoftware y FrancisJPK

/*-----( Importar Librerias )-----*/
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <TMAP.h>
/*-----( Declarar variables :D )-----*/

#define motor 1 //definir tipo de motor 0 = diesel ; 1 = nafta
#define dev 1   //habilita mensajes por serial
const String ver "## ver 0.01 Alpha ##";
int cil = 4;   //cantidad de cilindros del motor
//***REASIGNAR LUEGO TODOS LOS PINES***

/*-----( Variables INYECCION )-----*/

int iny[] = {3,4,5,6};       //Pines de arduino que esta conectados los inyectores **CAMBIAR PINES**

bool arr     = false;        //si se intenta arrancar el motor esta en true

int mar      = A1;           //pin de mariposa de acelerador
int marv     = 0;            //valor actual de mariposa de acelerador
bool iny     = false;       //variable para saber si se esta inyectando todavia o no

bool acl;                   //varible temporar, guarda si se intento acelerar el motor

int vuelta2  = 0;           //numero de vuelta para inyeccion
int dntact   = 0;			//diente actual del cigueñal
int dntPER   = 89;		    //cantidad de dientes entre inyeccion

int ACL_PER  = 15;			//cantidad de vueltas que ACL espera para cambiar el tiempo de inyeccion
int ACL_TMP  = 1500;		//cantidad de tiempo de inyeccion que ACL sube o baja

int ARR_TINY1= 25000; 		//tiempo de inyeccion en arranque frio
int ARR_PER  = 5; 			//primer periodo de vueltas, en este se espera que pase este valor con el tiempo base de arranque
int ARR_RPM  = 0;			//rpm para arranque

int ARR_FLAG2 = 800;		//cantidad de vueltas maximas para considerar que se intenta arrancar el motor
int ARR_FLAG1= 0;			//FLAG para control de arranque, sube de valor cada vez que pasaron 15 vueltas sin problemas de inyeccion
float ARR_t  = 0;			//temperatura para control de arranque

int CINY_RPM = 0;
int CINY_X;					//la uso para redondear rpm para rpm objetivo de acl

/*-----( Variables RPM )-----*/

int intervalo1 = 500;        //intervalo para medir rpm
int pul        = LOW;        //pulso de disparo de sensor de posicion de cigueñal
bool varrpm    = LOW;        //variable para no volver a contar mismo diente como pulso

int pinrpm     = 10;         //pin conectado al sensor de posicion de cigueñal
int diente     = 0;          //numero de diente actual de la corona
int dnt        = 20;         //cantidad de dientes que tiene el sensor de posicion de cigueñal

int rpm        = 0;          //numero actual de rpm
int rpmMIN     = 800;        //numero minimo de rpm
int rpmMAX     = 7000;       //numero maximo de rpm

int tolrpm     = 50;         //tolerancia de rpm
int vuelta     = 0;          //numero de vuelta para rpm
bool varr      = false;      //variable para evitar calcular rpm varias veces
int rpmant     = 0;          //rpm anteriores

int promedio   = 0;          //promedio tiempo entre dientes
int Tdnt[dnt];               //aca se guardan los tiempos entre dientes, luego se promedia (son 12);

unsigned long milirpm;       //tiempo previo ultimo periodo medicion rpm
unsigned long curMillis;     //tiempo actual del micro

bool sincronizado = false;   //en true si esta sincronizado la variable vuelta con el PMS
/*-----( Variables Temperatura )-----*/

int sensorT    = 3;          //pin de sensor Temperatura
int pinVENT    = 13;         //pin de control del ventilador

float temp     = 0;          //temperatura motor
float tempfrio = 40;         //temperatura maxima para considerar el motor "frio"
float tempmax  = 115;        //temperatura maxima de funcionamiento antes de entrar en modo emergencia

float tempvenMIN = 80;       //temperatura a la que se apaga el ventilador
float tempvenMAX = 95;       //temperatura a la que se enciende el ventilador

/*-----( Variables Encendido )-----*/


  /*-----( Variables Encendido )-----*/

  int avanceDeChispa        = 3; //en dientes de volante (2,42 grados) , default en 3
  int periodo               = 7; //periodo en mS
  unsigned long tiempo      = 0;
  unsigned long millisant   = 0;
  int pinBobinas14          = 8; //pines utilizados para mandar señales a las bobinas
  int pinBobinas23          = 9;
  bool activado             = false; //?
  bool arrancando           = false; //Esta arrancando?
  int tablaAvance[18][11]     ={/*RPM|TEMP   0 ;18;27;36;45;55;64;73;82;91;100 */
								/*800*/		{3 ,3 ,3 ,3 ,4 ,6 ,6 ,7 ,8 ,8 ,8 },
								/*1000*/	{3 ,3 ,3 ,4 ,5 ,8 ,8 ,8 ,10,10,10},
								/*1200*/	{5 ,5 ,5 ,5 ,6 ,9 ,9 ,10,11,12,12},
								/*1500*/	{8 ,8 ,8 ,8 ,8 ,10,10,11,13,14,14},
								/*1700*/	{10,10,10,10,10,11,11,12,14,15,15},
								/*2000*/	{12,12,12,12,12,13,13,14,15,16,16},
								/*2200*/	{13,14,14,14,14,15,15,16,17,18,18},
								/*2500*/	{15,16,16,16,16,17,17,17,18,19,19},
								/*2700*/	{17,18,18,18,18,19,19,19,20,21,21},
								/*3000*/	{19,20,20,20,20,21,21,21,22,23,23},
								/*3200*/	{21,22,22,22,22,23,23,23,24,25,25},
								/*3500*/	{23,24,24,24,24,25,25,25,26,27,27},
								/*3700*/	{25,26,26,26,26,27,27,27,28,29,29},
								/*4000*/	{26,28,28,28,28,29,29,29,30,31,31},
								/*4500*/	{28,30,30,30,30,31,31,31,32,33,33},
								/*5000*/	{30,32,33,33,33,34,34,34,35,36,36},
								/*5500*/	{32,34,34,34,34,34,34,35,37,38,38},
								/*6000*/	{34,35,35,35,35,35,35,36,38,39,39}
								}				 //matriz tabla de avance
/*-----( Variables Logica )-----*/
int var  = 0;               //variable usada para bucles
int varX = 0;               //variable temporal, multiples usos

/*-----( Otras variables )-----*/
float presB = 1.75; //presion minima de sensor TMAP
float presA = 2.10; //presion maxima de sensor TMAP

TMAP myTMAP = TMAP(A2,A3,presB, presA); //iniciamos sensor TMAP

int pinLuzMuerte = 12;       //sip, pin de luz de "check Engine"
bool emergencia  = false;    //mientras que este en false todo bien ^~^
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // definimos el display I2C
const uint8_t charBitmap[][8] = { 
    //array con simbolo de grados en diferentes lugares :P
   { 0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0 },
   { 0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0 },
   { 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0 } 
};
void int0(){ //interrupcion, se ejecuta cada vez que el sensor de posicion de cigueñal cambia
    if(emergencia == false){ //si entro en modo emergencia no hacemos caso a nada
        varr = false;
        vuelta++;
        vuelta3++;
        if(vuelta >= perinyec){
              vuelta2++;
              per = true;
        }
    }
}

void setup(){
   #if(dev == 1) 
   Serial.begin(14400); //iniamos comunicacion serial
   #endif
    //definir E/S
    pinMode(pinrpm, INPUT);
    pinMode(sensorT, INPUT);
    pinMode(mar, INPUT);
    pinMode(Lamb, INPUT);
    pinMode(ARRpin, INPUT);
    pinMode(pinLuzMuerte, OUTPUT);
    pinMode(pinVENT, OUTPUT);
    #if(dev == 1) 
        Serial.println("Se termino de definir E/S");
   #endif
    int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));
  #if (motor == 1)
    pinMode(pinBobinas13, OUTPUT);
    pinMode(pinBobinas24, OUTPUT);
    //definimos estado de las bobinas
    digitalWrite(pinBobinas13, LOW);
    digitalWrite(pinBobinas24, LOW);
  #endif
    for(var = 0; var >= 3; var++){
    //definimos inyectores como salida
        pinMode(iny[var], OUTPUT);}

    for(var = 0; var >= 3; var++){
        digitalWrite(iny[var], LOW);}
    #if(dev == 1) 
        Serial.println("DBG Iniciando Sistema...");
   #endif
     //mostramos mensaje en LCD :D
    lcd.setCursor(0,0);
    lcd.print("####################");
    lcd.setCursor(0,1);
    lcd.print("##### OpenEFI ######");
    lcd.setCursor(0,2);
    lcd.print(ver);
    lcd.setCursor(0,3);
    lcd.print("####################");
    delay(500);
    LCD(5,0,0);

	if(!SD.begin(12)){
		lcd.setCursor(0,3);
		lcd.print("No se encuentra  SD");
        delay(500);
        emerg = true;
}
    //definir interrupciones HW
     attachInterrupt(digitalPinToInterrupt(2), int0, RISING);
    interrupts();
}

void loop(){
    #if(dev == 1) 
        Serial.println("DBG Listo");
   #endif
    for(;;){ //Remplazo del loop para ganar rendimiento
        ControlRPM();
        marv = analogRead(mar) / 4;
        temp = sensortemp();
        Temperatura();
        ControlINY(varINY);
        controlDeEncendido(temp);
        ControlPWM();
        int rpmC = RPM();
        Serial.write("RPM ");
        Serial.writeLine(rpmc)
    }
}

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

void ControlINY(){
	CINY_RPM = RPM();
	if(acl = false){
		if(CINY_RPM <= ARR_FLAG2 && CINY_RPM != 0){
			ARR_FRIO();
		}else if(CINY_RPM >= ARR_FLAG2 && CINY_RPM != 0){
			inyT = Tiny(CINY_RPM, marv); 
			 CINY_X = map(map(rpm2,0,7000,0,70)),
			CINY_X = CINY_X * 100;
			acl = ACL(CINY_X, inyT);
		}
	}else{acl = ACL(CINY_X, inyT);}
}

void ARR_FRIO(){//Sub-programa,controla tiempo de inyeccion hasta alcanzar 1500 rpm x 1500 vueltas
	inyT = ARR_TINY1;
	for(vuelta2 > ARR_PER){
		ControlPWM();
		float ARR_t = sensortemp();
		controlDeEncendido(t);
	}
	do{
		ARR_RPM = RPM();
		if(!ACL(1500,ARR_RPM,ARR_TINY1)){
			ARR_FLAG1++;
		}
		ControlPWM();
		controlDeEncendido(t);
	}while(ARR_FLAG1 >= 10);
}


bool ACL(int rpmOBJ, int TBASE){
	int ACL_RPM = RPM() - rpmOBJ;
	if(ACL_RPM >= 400 &&  vuelta2 > ACL_PER){ //diferencia mayor a 400 vueltas
		TBASE = TBASE - ACL_TMP; // sacamos 1.5mS de tiempo de inyeccion
		vuelta2 = 0;
		return true;
	} else if(ACL_RPM <= 400 && vuelta2 > ACL_PER){
		TBASE = TBASE + ACL_TMP;
		vuelta2 = 0;
		return true;
	}else if(){// ya no hace falta cambiar el tiempo desde esta funcion, esto habilita correccion por sonda lambda y TMAP
		acl = false;
		vuelta2 = 0;
		return false;} 
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



void ControlPWM(){
    //cambiar esto para hacerlo cada X° del cigueñal o dientes
    if(emergencia == false){ //mientras que no estemos en modo de emergencia V:

        if (inyectando == false  && per == true){
          digitalWrite(iny[vuelta2++],HIGH);
            unsigned long tmpINY = micros();
            inyectando = true;
            per = false; //per es para controlar que se inyecte una sola vez :P
        }
      if (micros() - tmpINY >= inyT) { //Cuando pase el tiempo de inyeccion
                digitalWrite(iny[vuelta2++],LOW); //apagamos el inyector :P
        if(vuelta2 == (cilindros - 1){ //cuando la cantidad de vueltas sea igual al n° de cilindros
          vuelta2 == 0;
        }
        inyectando == false;
      }

    }
}


#if (motor == 1)
+//------------------------(FrancisJPK inicio)-----------------------------------------
+void controlDeEncendido(float temperatura){
+
+    //----CONTROL DE AVANCE ANTES DE MANDAR CHISPA----
+    if(arrancando == true){
+		//controlador de avance en frio
+        avanceArranque();
+    }else if(arrancando == false){
+		avanceNormal(temperatura);
+    }
+    //---------------FIN DE CONTROL DE AVANCE PARA PROCEDER A MANDAR CHISPA-----------
+	//--------------------------------------------------------------------------------
+    //----------------------------------CONTROL DE MANDADA DE CHISPA-----------------
+        if(diente >= ((dnt/4) - avanceDeChispa) && diente <= ((dnt/4)-avanceDeChispa)+1){
+            iniciarChispazo(pinBobinas14);
+            activado = true;
+            millisant = millis();
+            //----chispazo para el piston 1 y 4(siendo el 4 chispa perdida)
+        }else if(diente >= ((dnt/2) - avanceDeChispa) && diente <= ((dnt/2)-avanceDeChispa)+1){
+            iniciarChispazo(pinBobinas23);
+            activado = true;
+            millisant = millis();
+            //----chispazo para el piston 2 y 3(siendo el 2 chispa perdida)
+        }else if(diente >= (((dnt/4)*3) - avanceDeChispa) && diente <= (((dnt/4)*3)-avanceDeChispa)+1){
+            iniciarChispazo(pinBobinas14);
+            activado = true;
+            millisant = millis();
+            //----chispazo para el piston 1 y 4(siendo el 1 chispa perdida)
+        }else if(diente >= (dnt - avanceDeChispa) && diente <= (dnt - avanceDeChispa)+1 ){
+            iniciarChispazo(pinBobinas23);
+            millisant = millis();
+            activado = true;
+            //----chispazo para el piston 2 y 3(siendo el 3 chispa perdida)
+        }
+
+        if ((millis() - millisant) >= periodo && activado == true) {
+            millisant = millis();
+            activado = false;
+            pararChispazo(pinBobinas14);
+            pararChispazo(pinBobinas23);
+        }
+}//------------------FIN DE CONTROL DE MANDADA DE CHISPA-------------------------
+//------------------------------------------------------------------------------
+//--------------------BLOQUE DE CONTROLADORES DE AVANCE--------------------------
+
+void avanceArranque(){
+	avanceAnterior = avanceDeChispa;
+	avanceDeChispa = dientes(3);
+	if(avanceAnterior != avanceDeChispa){
+		LCD(2,avanceDeChispa,0);
+	}
+}
+void avanceNormal(float temperatura){
+
+	int indiceTemp = 0;//indice que indica a que columna de la matriz acceder(temperatura)
+	int indiceRPM = 0;//indice que indica a que fila de la matriz acceder(RPM)
+
+	//------------BLOQUE DE ASIGNACION DE INDICES A LEER EN TABLAS------------
+	if{temperatura < 98}{
+		indiceTemp = ((temperatura < 18)? 0:((temperatura < 27)? 1:((temperatura < 36)? 2:((temperatura < 45)? 3:((temperatura < 55)? 4:((temperatura < 64)? 5:((temperatura < 73)? 6:((temperatura < 82)? 7:((temperatura < 91)? 8:9)))))))));
+		//este bodoque gigante de operaciones ternarias es para asignar el indice a leer en tabla segun la temperatura
+		//este indice corresponde a las columnas (eje X)
+	}else{
+		indiceTemp = 10;
+	}
+	indiceRPM = ((RPM<1000)?0:((RPM<1200)?1:((RPM<1500)?2:((RPM<1700)?3:((RPM<2000)?4:((RPM<2200)?5:((RPM<2500)?6:((RPM<2700)?7:((RPM<3000)?8:((RPM<3200)?9:((RPM<3500)?10:((RPM<3700)?11:((RPM<4000)?12:((RPM<4500)?13:((RPM<5000)?14:((RPM<5500)?15:((RPM<6000)?16:17))))))))))))))))); 
+	//este otro bodoque gigante de operaciones ternarias es para asignar el indice a leer en tabla segun las RPM
+	//este indice corresponde a las filas (eje Y)
+	//---------
+	avanceAnterior = avanceDeChispa;
+	avanceDeChispa = dientes(tablaAvance[indiceRPM][indiceTemp]);
+	//finalmente accedemos al valor en tabla correspondiente al estado actual del motor
+	if(avanceAnterior != avanceDeChispa){
+		LCD(2,avanceDeChispa,0);
+	}
+	//-----------FIN DEL BLOQUE DE ASIGNACION DE INDICES A LEER EN TABLAS-----
+}
+//--------------------FIN DE BLOQUE DE CONTROLADORES DE AVANCE----------------
+//-----------------------------------------------------------------------------
+//---------------------FUNCIONES DE CONTROL DE BOBINAS------------------------
+void iniciarChispazo(int pin){//inicia la chispa hasta que se llame al metodo pararChispazo()
+    digitalWrite(pin, HIGH);
+}
+void pararChispazo(int pin){//para la chispa
+    digitalWrite(pin, LOW);
+}
+//---------------------(FrancisJPK final)---------------------------------------------------
#endif

void Temperatura(){
    //Controla el ventilador y apagado de emergencia del motor
    if(temp >= tempmax){
        inyT = 0;
        emergencia = true;
        emerg();
    }
    if(temp <= tempvenMIN){
        digitalWrite(pinVENT, LOW);
    }
    if(temp >= tempvenMAX){
        digitalWrite(pinVENT, HIGH);
    }
}



float  sensortemp(){
  //esta funcion mide la temperatura y devuelve float en celsius
  //**solo para sensor LM35**
    int value = analogRead(sensorT);
    float millivolts = (value / 1023.0) * 5000;
    float celsius = millivolts / 10; 
    return celsius;
} 
//funcion para el control del lcd
//modificar luego para actualizar solo valores que cambian no toda la linea
void LCD(int op, float texto, int texto2){
    String linea1 = "###OpenEFI v0.010###";
    String lin2  = "#RPM: ";
    String lin2a = "#T:";
    String lin2b = "#";
    String lin3a = "###T.INY:";
    String lin3b = " mS###";
    String lin4a = "###Avance: ";
    String lin4b = "###";
    String LX;
    if(op == 0){
         LX = lin2 + texto2 + lin2a + temp;
        lcd.setCursor(0,1);
        lcd.print(LX);
        lcd.setCursor(17,1);
        lcd.print (char(random(7)));
        lcd.setCursor(18,1);
        lcd.print("C#");
        lcd.print(LX);
    }
    if(op == 1){
         LX = lin3a + texto + lin3b;
        lcd.setCursor(0,2);
        lcd.print(LX);
    }
    if(op == 2){
        LX = lin4a + texto;
        lcd.setCursor(0,3);
        lcd.print(LX);
        lcd.setCursor(16,3);
        lcd.print (char(1));
        lcd.setCursor(17,3);
        lcd.print(lin4b);
    }
    if(op == 5){ //esta opcion es para inciar el panel
        lcd.setCursor(0,0);
        lcd.print("###OpenEFI v0.010###");
        lcd.setCursor(0,1);
        lcd.print("# RPM:0000 #T 000");
        lcd.setCursor(17,1);
        lcd.print (char(1));
        lcd.setCursor(18,1);
        lcd.print("C#");
        lcd.setCursor(0,2);
        lcd.print("### T.INY: 00  mS###");
        lcd.setCursor(0,3);
        lcd.print("#### Avance: 00");
        lcd.setCursor(16,3);
        lcd.print (char(random(7)));
        lcd.setCursor(17,3);
        lcd.print(lin4b);
    }
}
//convierte grados en dientes del sensor hall
int dientes(float grados){
    float grad = 360 / dnt; //dividimos 360 grados por la cantidad de dientes
    //dividimos grados por grad, luego multiplicamos por 100 para transformar el float en int
    int x2     = (grados / grad) * 100; 
    //dividimos por 100, al hacer esto se eliminan los decimales, en prox ver redondear
    int dnt2    = x2 / 100;
    return dnt2; 
}
//funcion en caso de emergencia del motor
void emerg(){
    #if(dev == 1) 
        Serial.println("DBG Motor en estado de emergencia");
   #endif
    noInterrupts(); //desactivamos interrupciones
    digitalWrite(pinLuzMuerte, HIGH);
    int varemerg = 1;
    lcd.setCursor(0,0);
    lcd.print("####################");
    lcd.setCursor(0,1);
    lcd.print("#### EMERGENCIA ####");
    lcd.setCursor(0,2);
    lcd.print("## Motor detenido ##");
    lcd.setCursor(0,3);
    lcd.print("####################");
    while(varemerg == 1){
        delay(1000);
    }
}

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
    for(int i = 0; i < dnt; i++;){ //i menor a dnt porque empieza la cuenta en 0 el for y los dientes no
        if (Tdnt[i2] < TMIN){
            TMIN = Tdnt[i2];}
        if (Tdnt[i2] > TMAX){
            TMIN = Tdnt[i2];}
        i2++;
    }
    vuelta = 0;
    sincronizado = true;
}

float distanciaEntreDientes(int revolucionesPM){
    //ESTA FUNCIÓN DEVUELVE LA DISTANCIA ENTRE DIENTES EN MILISEGUNDOS
    float calculo = ((rpm/60) * (dnt)) / 1000;
   
    //dividimos las rpm en 60 segundos
    //lo que nos da las vueltas por segundo, lo multiplicamos por dientes del volante
    //para que nos diga cuantos dientes pasan en un segundo. Lo dividimos por 1000
    //para que nos diga cuantos dientes pasan por milisegundo
    //al hacer "1/calculo" nos da la distancia entre dientes en milisegundos
   
    return (1/calculo);//SE DEVUELVE LA DISTANCIA ENTRE DIENTES EN MS
}
#if (dev == 1) //si se activo la depuracion
int freeRam () { //funcion para saber la ram disponible
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif