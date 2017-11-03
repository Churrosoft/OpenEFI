/*###############################################
########### Fixed Mode Loop & Control############  
#################################################*/

void FXM(){
    //este seria seudo loop que tendria el modo fijo,
    bool _FM = true;
    do{
        C_PWM();
        HILO_1();
        vent();
        if(_msg){
            _msg = false;
        }
    }while(_FM);
}