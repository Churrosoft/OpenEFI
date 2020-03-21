#include "../variables.h"
#include <string.h>

#include "./hash.c"
#include "./codes.h"

//Variables de todo el socotroco:
char buf3[256] = "";
int dataSize = 0;
// Declaracion de funciones:
int process(char *data);
int get_data(char *tempbuf, int len);
char *getMSG(void);
void clearMSG(void);
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

int get_data(char * tempbuf , int len){

    if ((dataSize + len) < 256) {
        dataSize = strlen(buf3) + len;
        strcat(buf3, tempbuf);
       // memset(tempbuf, 0, sizeof tempbuf);
    }
    if (buf3[dataSize - 1] == '\n' || dataSize > 250){
        return dataSize;
    }

    return 0;
}

char * getMSG(){
    return buf3;
}

void clearMSG(){
    memset(buf3, 0, dataSize);
    dataSize = 0;
}