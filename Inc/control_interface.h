#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

// Definición de mensajes
typedef struct {
    // Versión del protocolo. Por ahora PROTOCOL_VERSION_1.
    uint8_t protocolVersion;
    // Comando a ejecutar.
    uint8_t command;
    uint8_t subcommand;
    uint8_t payload[123];
    // CRC16 del mensaje
    uint16_t checksum;
} SerialMessage;

bool get_frame(uint8_t*, int);
uint8_t *get_msg(void);
void clear_msg(void);
void send_message(/* usbd_device*, */ SerialMessage*);
void process_frame(/* usbd_device*, */ SerialMessage*);
uint16_t crc16(uint8_t*, uint8_t);
#endif