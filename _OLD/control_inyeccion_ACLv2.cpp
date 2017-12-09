/*-----( Variables _LMB )-----*/
bool LMBM = false; //en true si se utilizan las dos sondas lambda
bool LMBM2= false; //en true si esta en ciclo cerrado, sino ciclo abierto
int
    LMBA  = A4,
    LMBB  = A5,
    CTA   = 250, //Correcion de tiempo A, mezcla rica, se le sacan X uS
    CTB   = 300; //Correcion de tiempo B, mezcla pobre, se le agregan X uS

float FLMBA = 1.5,  //factor maximo de lambda
      FLMBB = 0.85; //factor minimo de lambda


void C_INY(){
    //funcion que maneja el control de la inyeccion del motor
    if(_TEMP >= 46 && !ACL){
        INYT1 = Tiny(_RPM, _MAR,1);
    }
    if(_RPM < _RLT && !ACL){
        INYT1 = Tiny(_RPM, _MAR,3);
    }
}

int Tiny(int rpm2, int marv2, int OP){
    //esto hace toda la magia :V
    //con esta funcion se obtiene el tiempo base de inyeccion y luego se puede...
    //recalcular dependiendo de la necesidad
	int rpm3  = map(rpm2,0,7000,0,17); //aproximamos las rpm
	int marv3 = map(marv2,0,255,0,10); //aproximamos el valor de la mariposa de aceleracion
	switch(OP){ 
        case 0://Tiempo de inyeccion base
            return TINY[marv3,rpm3];
         break;

        case 1://Tiempo de inyeccion base + correccion por lambda
            return _LMB(TINY[marv3,rpm3]);
         break;
         case 2: //ALPHA-N con MAP
            return _APHPLUS();
         break;
         case 3: //Tiempo de inyeccion para arrancar
            return INY_L + (INY_P *2);
         break;
    }
}

int _APHPLUS(){ //ALPHA-N con MAP
/*
 PW = INJ_CONST * VE(tps,rpm) * MAP * AirDensity + AccEnrich +InjOpeningTime

 PW (pulse width) ---el tiempo final de apertura del inyector .

 INJ_CONST ----Es el valor constante , que determina el tiempo de apertura para que se crea la mezcla estequiométrica (lambda=1 ) , para cilindrada del motor , presión a 100kPa , temperatura del aire a 21ºC y VE 100% .

 VE para Speed Density (map, rpm)----Valor de eficiencia volumétrica tomada de la mapa VE .

 VE para ALPHA-N (tps, rpm)----Valor de eficiencia volumétrica tomada de la mapa VE .

 MAP(manifold absolute pressure)--- Presión tomada en el colector de admisión .

 AirDensity ---- Porcentual diferencia de densidad de aire comparada con la densidad de aire a temperatura 21ºC

 AccEnrich (acceleration enrichment) ---- Enriquecimiento de la mezcla en fase de aceleración .

 InjOpeningTime ---- Tiempo de apertura de inyector hasta el momento de inicio de inyección de combustible ( valor de retardo tomado de la mapa de calibración , INJECTORS CAL. )
*/
 return INY_C * VE[_MAP, _rpm] * _MAP * 1.20 + INY_P + INY_L ;
}
int _LMB(int T){

    //Esta funcion mide la/s sonda lambda y corrige la mezcla / 
    //tiempo de inyeccion
    float _LMB_XA = analogRead(LMBA) * (3.3 / 1023); //medimos la sonda A
    float _LMB_XB = 0;
    //V menor a 0.45 = mezcla pobre
    if(_LMB_XA < 0.45){ //aca comparo por Volt pero tendriaaaa que hacerlo con los el factor lamba :S
        T = T + CTB;
    } else if(_LMB_XA > 0.45){ //V mayor a 0.45 = mezcla rica
        T = T - CTA;
    }
    if(LMBM){ 
        _LMB_XB = analogRead(LMBB) * (3.3 / 1023); //medimos la sonda B (Si corresponde)
        //V menor a 0.45 = mezcla pobre
        if(_LMB_XB < 0.45){ //aca comparo por Volt pero tendriaaaa que hacerlo con los el factor lamba :S
             T = T + CTB;
        } else if(_LMB_XB > 0.45){ //V mayor a 0.45 = mezcla rica
            T = T - CTA;
        }
    }
    return T;
}