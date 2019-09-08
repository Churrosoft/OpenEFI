
void pinMode(int PORT, int PIN, bool MODE){
    if(MODE){
        gpio_set(PORT, PIN);
	    gpio_set_mode(PORT, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, PIN);
    }
}