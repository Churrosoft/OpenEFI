/*###############################################
########### Fixed Mode Loop & Control############  
#################################################*/

void FXM(){
    //este seria seudo loop que tendria el modo fijo,

    bool _FM = true;
    String _t = "";
    if(_msg && msg.startsWith("FXD.E")){
        do{
            C_PWM();
            HILO_1();
            vent();

            if(_msg){
                if(msg.startsWith("FXD.D")){ // "FX.D" Termina el modo fijo de inyeccion
                    _FM = false;
                }else if(msg.startsWith("INY")){ // "INY" cambia el tiempo de inyeccion
                    _t = msg.erase(0,3);
                    INYT1 = _t.toInt();
                }else if(msg.startsWith("AVC")){ // "AVC" cambia el tiempo de avance
                    _t = msg.erase(0,3);
                    AVC = _t.toInt();
                }

                _msg = false;
            }
        }while(_FM);
    }
}