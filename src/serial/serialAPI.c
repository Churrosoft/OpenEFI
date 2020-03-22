#include "../variables.h"
#include <string.h>

#include "./hash.c"
#include "./codes.h"

//Variables de todo el socotroco:
struct serialAPI{
    char buffer[128];
    int dataSize;
} mySerial = {
    {}, 0
};

#define PROTOCOL_VERSION_1 1

#define COMMAND_PING 1
#define COMMAND_TMP 2
#define COMMAND_RPM 3
#define COMMAND_00V 4
#define COMMAND_AVC 5
#define COMMAND_INY 6
#define COMMAND_LMB 7
#define COMMAND_DBG 8
// Enviado cuando ocurre un error. El error se especifica en el subcomando
#define COMMAND_ERR 8
#define ERROR_INVALID_PROTOCOL 1
#define ERROR_INVALID_COMMAND 2

#define SUBCOMMAND_NONE 0

// Definición de mensajes
typedef struct {
    // Versión del protocolo. Por ahora PROTOCOL_VERSION_1.
    uint8_t protocolVersion;
    // Comando a ejecutar.
    uint8_t command;
    uint8_t subcommand;
    char payload[123];
    // CRC16 del mensaje
    uint16_t checksum;
} SerialMessage;


// Declaracion de funciones:
int process(char *data);
bool get_frame(char *tempbuf, int len);
char *get_msg(void);
void clear_msg(void);
int get_data_size(void);
void send_message(usbd_device*, SerialMessage*);

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

void send_message(usbd_device *usbd_dev, SerialMessage* message){
    usbd_ep_write_packet(usbd_dev, 0x82, message, 64);
    for (int i = 0; i < 0x8000; i++) __asm__("nop");
    usbd_ep_write_packet(usbd_dev, 0x82, (message + 64), 64);
}

bool get_frame(char *tempbuf, int len){
    if ((mySerial.dataSize + len) <= 128){
        memcpy(mySerial.buffer + mySerial.dataSize, tempbuf, len);
        mySerial.dataSize += len;
        if(mySerial.dataSize >= 128){
            return true;
        }
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