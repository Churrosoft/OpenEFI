class VAR(){
private:
/*-----( Variables INYECCION )-----*/
public:
int iny[]    = {3,4,5,6};    //Pines de arduino que esta conectados los inyectores **CAMBIAR PINES**
int inyT     = 0;            //tiempo de inyeccion   
bool arr     = false;        //si se intenta arrancar el motor esta en true

int mar      = A1;           //pin de mariposa de acelerador
int marv     = 0;            //valor actual de mariposa de acelerador
bool inye     = false;       //variable para saber si se esta inyectando todavia o no

bool acl;                   //varible temporar, guarda si se intento acelerar el motor
bool per;

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
int Tdnt[100];               //aca se guardan los tiempos entre dientes, luego se promedia (son 12);
int perinyec = 40;           //cantidad de dientes entre inyeccion
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
								};				 //matriz tabla de avance
/*-----( Variables Logica )-----*/
int var  = 0;               //variable usada para bucles
int varX = 0;               //variable temporal, multiples usos

/*-----( Otras variables )-----*/
char *lin; int indice = 0; //las uso para control de linea SD
char linea[200];
float presB = 1.75; //presion minima de sensor TMAP
float presA = 2.10; //presion maxima de sensor TMAP
String datoSerie = "";
boolean SerieCompl = false;
TMAP myTMAP(A2,A3); //iniciamos sensor TMAP
int i = 0;
int pinLuzMuerte = 12;       //sip, pin de luz de "check Engine"
bool emergencia  = false;    //mientras que este en false todo bien ^~^
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // definimos el display I2C
const uint8_t charBitmap[][8] = { 
    //array con simbolo de grados en diferentes lugares :P
   { 0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0 },
   { 0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0 },
   { 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0 } 
};

};