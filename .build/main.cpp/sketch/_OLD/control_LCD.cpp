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