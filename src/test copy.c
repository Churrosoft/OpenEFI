#include <stdio.h>
#include <string.h>

void test(){
    char buf[256] = "", tempbuf[64] = "";
	int fall = 0, dataSize = 0;
    int len= 0;
    
	do{
		len = usbd_ep_read_packet(usbd_dev, 0x01, tempbuf, 64);

		if(len && ( (dataSize + len ) < 256 ) ){
		    dataSize = strlen(buf) + len;
			strcat(buf,tempbuf);
		}
		else if ( ( dataSize + len ) > 256 ){
			break;
		}else {
			fall++;
		}

	}while( fall < 3 && buf[dataSize - 1] != '\n'  &&  dataSize < 256);

	if (dataSize > 0){
		char str[64]; //si son mas de 64 bytes de una patada el usb no se banca la joda
		sprintf(str, "%d", dataSize);
		usbd_ep_write_packet(usbd_dev, 0x82, str, strlen(str));
	} 
}

int main(){
    
    test();
    printf("Hello World");

    return 0;
}
