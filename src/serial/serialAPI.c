#include "../variables.h"
#include <string.h>

#include "./hash.c"
#include "./codes.h"

//Variables de todo el socotroco:
struct serialAPI{
    char buffer[256];
    int dataSize;
}mySerial = {
    "" ,0
};


// Declaracion de funciones:
int process(char *data);
bool get_data(char *tempbuf, int len);
char *getMSG(void);
void clearMSG(void);
int getDataSize(void);

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

bool get_data(char *tempbuf, int len){

    if ((mySerial.dataSize + len) < 256){

        memcpy(mySerial.buffer + mySerial.dataSize, tempbuf, len - 1);
        mySerial.dataSize += len - 1;
    }
    if (tempbuf[strlen(tempbuf) - 1] == '\n'){
        return true;
    }
    return false;
}

char *getMSG(){
    return mySerial.buffer;
}

void clearMSG(){
    memset(mySerial.buffer, 0, mySerial.dataSize);
    mySerial.dataSize = 0;
}

int getDataSize(){
    return mySerial.dataSize;
}