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