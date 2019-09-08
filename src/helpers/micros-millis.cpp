uint32_t microsA;
uint32_t microsB;
uint32_t millisA;

void sys_tick_handler(void){
	microsA++;
  microsB++;

	if (microsA == 1000) {
		millisA++;
		microsA = 0;
	}

}

uint32_t micros();

uint32_t micros(){
  return microsB;
}

uint32_t millis();

uint32_t millis(){
  return millisA;
}