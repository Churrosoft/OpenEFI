void Temperatura(){
    //Controla el ventilador y apagado de emergencia del motor
    if(temp >= tempmax){
        inyT = 0;
        emergencia = true;
        emerg();
    }
    int temp2 = int(temp); //pasamos temperatura a int
    int mapatemp = map(temp2, -15 , 120, 0,255); //con esto convertimos el rango de temperatura en un rango de valores para pwm
    analogWrite(pinVENT,mapatemp);
}


float  sensortemp(){
  //esta funcion mide la temperatura y devuelve float en celsius
  //**solo para sensor LM35**
    int value = analogRead(sensorT);
    float millivolts = (value / 1023.0) * 5000;
    float celsius = millivolts / 10; 
    return celsius;
} 