#include "interfazSerial.h"
#include <Arduino.h>

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

void interfazSerial::processInput(char* input){
	char *command = strtok(input, " ");
	char *params = strtok(NULL, " ");
	if(strcmp(command, "NOP") == 0){
		interfazSerial::send("ACK", command);
	}else{
		interfazSerial::send("NAK", input);
	}
}