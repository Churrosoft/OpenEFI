#include <stdio.h>
#include <string.h>

void test(){
    char buf[256] = "string sin EOF ", tempbuf[64] = "el señor EOF: \n";
	int fall = 0, dataSize = 0;
    int len= 0;
    
	do{
		len = strlen(tempbuf);

		if(len && ( (dataSize + len ) < 256 ) ){
		    dataSize = strlen(buf) + len;
			strcat(buf,tempbuf);
		}
		else if ( ( dataSize + len ) > 256 ){
			break;
		}else {
			fall++;
			for (i = 0; i < 0x800; i++)
				__asm__("nop");
		}
	

	}while( fall < 15 && buf[dataSize - 1] != '\n'  &&  dataSize < 256);

    
	if (dataSize > 0){
	    printf("largo de trama: \"%u\" \n",dataSize);
		printf("contenido de trama: %s",buf);
	} 
}

int main(){
    
    test();
    printf("Hello World");

    return 0;
}
