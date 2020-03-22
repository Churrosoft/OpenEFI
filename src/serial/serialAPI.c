#include "../variables.h"
#include <string.h>

#include "./hash.c"
#include "./codes.h"

//Variables de todo el socotroco:
struct serialAPI{
    char buffer[256];
    int dataSize;
}mySerial = {
    "" ,1
};


// Declaracion de funciones:
int process(char *data);
bool set_data(char *tempbuf, int len);
char *get_msg(void);
void clear_msg(void);
int get_data_size(void);

//--------------------------
int process(char *data){
    switch (CRC(data, 8)) {
        case RPM_CODE:
            return _RPM;
            break;
        case TMP_CODE:
            return _TEMP;
            break;
        case 4:
            return 34;
            break;
    default:
        return -4;
        break;
    }
}

bool set_data(char *tempbuf, int len){

    if ((mySerial.dataSize + len) < 256){
        memcpy(mySerial.buffer + mySerial.dataSize, tempbuf, len);
        mySerial.dataSize += len;
    }
    if (tempbuf[strlen(tempbuf) - 1] == '\n'){
        return true;
    }
    return false;
}

char *get_msg(){
    return mySerial.buffer;
}

void clear_msg(){
    memset(mySerial.buffer, 0, mySerial.dataSize);
    mySerial.dataSize = 0;
}

int get_data_size(){
    return mySerial.dataSize;
}