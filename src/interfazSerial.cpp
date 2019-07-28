#include "interfazSerial.h"
#include <Arduino.h>

extern int _RPM;

interfazSerial::interfazSerial(){
	Serial.begin(115200);
}

void interfazSerial::send(String command, String message){
	Serial.print(command + " " + message + "\n");
}

void interfazSerial::send(String command, int message){
	Serial.print(command + " " + message + "\n");
}

void interfazSerial::query(){
	// Lee la entrada serial y procesa comandos
    static byte ndx = 0;
    char rc;
    
    while (Serial.available() > 0){
        rc = Serial.read();
        if(rc != '\n'){
            inputBuffer[ndx] = rc;
            ndx++;
            if (ndx >= 32) {
                ndx = 31;
            }
        }else{
            inputBuffer[ndx] = '\0'; // terminate the string
            ndx = 0;
			// Parse command
			interfazSerial::processInput(inputBuffer);
        }
    }
}

void interfazSerial::rpm(){
    if(config & 0b00000001){
        interfazSerial::send(F("RPM"), _RPM);
    }
}

void interfazSerial::processInput(char* input){
	char *command = strtok(input, " ");
	char *params = strtok(NULL, " ");
	if(strcmp(command, "NOP") == 0){
		interfazSerial::send(F("NOP"), "");
    }else if(strcmp(command, "RPM") == 0){
        if(params[0] == '\0'){
            interfazSerial::send(F("RPM"), _RPM);
        }else if(strcmp(params, "ON") == 0){
            config |= 0b00000001;
        }else if(strcmp(params, "OFF") == 0){
            config &= 0b11111110;
        }
    }else{
		interfazSerial::send(F("NAK"), input);
        return;
	}
}