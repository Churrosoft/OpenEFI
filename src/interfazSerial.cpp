#include "interfazSerial.h"
#include <Arduino.h>

interfazSerial::interfazSerial(){
	Serial.begin(115200);
}
