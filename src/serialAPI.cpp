#include "variables.h"
#include <algorithm>

#define RPM_CODE "000;RPM"
#define TMP_CODE "000;TMP"


int process(char* data){
    //a base del string de entrada me fijo que corno responder ?)
    if(strstr(data,RPM_CODE) != NULL){
        return _RPM;
    }else if (strstr(data,TMP_CODE) != NULL) {
        return _TEMP;
    }
    return -1;
}