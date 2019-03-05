 void ControlSerie(){
    //esta funcion se encarga de manejar los datos que llegan por serialEvent
    if(SerieCompl){
        SerieCompl = false;
    }
}

void serialEvent(){
    while(Serial.available()){
        char buff = (char)Serial.read();
        if(buff == '\n'){
            SerieCompl = true;
        }else {datoSerie += buff;}
    }
    ControlSerie();
}