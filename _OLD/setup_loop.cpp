void setup(){
   #if(dev == 1) 
   Serial.begin(14400); //iniamos comunicacion serial
   #endif
   datoSerie.reserve(200);
    //definir E/S
    pinMode(pinrpm, INPUT);
    pinMode(sensorT, INPUT);
    pinMode(mar, INPUT);
    pinMode(pinLuzMuerte, OUTPUT);
    pinMode(pinVENT, OUTPUT);
    #if(dev == 1) 
        Serial.println("Se termino de definir E/S");
   #endif
    int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));
  #if (motor == 1)
    pinMode(13, OUTPUT);
    pinMode(24, OUTPUT);
    //definimos estado de las bobinas
    digitalWrite(13, LOW);
    digitalWrite(24, LOW);
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
  // LCD(5,0,0); invalid cast to abstract class type 'LCD'

	if(!SD.begin(12)){
		lcd.setCursor(0,3);
		lcd.print("No se encuentra  SD");
        delay(500);
        emergencia = true;
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
        ControlINY();
        controlDeEncendido(temp);
        ControlPWM();
        int rpmC = RPM();
        Serial.print("RPM ");
        Serial.println(rpmC);
    }
}