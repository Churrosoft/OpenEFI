void ARR_FRIO(){//Sub-programa,controla tiempo de inyeccion hasta alcanzar 1500 rpm x 1500 vueltas
	inyT = ARR_TINY1;
	do{
		ControlPWM();
		float ARR_t = sensortemp();
		controlDeEncendido(ARR_t);
	}while(vuelta2 > ARR_PER);
	do{
		ARR_RPM = RPM();
		if(!ACL(1500,ARR_TINY1)){
			ARR_FLAG1++;
		}
		ControlPWM();
		controlDeEncendido(ARR_t);
	}while(ARR_FLAG1 >= 10);
}