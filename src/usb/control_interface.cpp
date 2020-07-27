/* USB Control Interface command processing. */
#include <string.h>
#include <libopencm3/cm3/scb.h>

#include "commands.h"
#include "defines.h"
#include "variables.h"
//Structs para formatear data:
#include "./dataStruct/status.h"
extern "C"
{
#include "../helpers/bootloader.h"
}
#include "control_interface.h"
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include "sensors/sensors.hpp"

//Variables de todo el socotroco:
char frameBuffer[128] = {};
int buffLength = 0;
struct usb_spam_interface usb_spam = {0, 0};

/** Procesa comandos.
 * @param usbd_dev
 * @param message Mensaje recibido.
 */
void process_frame(usbd_device *usbd_dev, SerialMessage *message)
{
    SerialMessage response = {PROTOCOL_VERSION_1, 0, 0, {}, 0};
    response.protocolVersion = PROTOCOL_VERSION_1;
    response.command = message->command;
    uint16_t localCRC = crc16((unsigned char *)message, 126);
    if (localCRC != message->checksum)
    {
        response.command = COMMAND_ERR;
        response.subcommand = ERROR_INVALID_CHECKSUM;
        send_message(usbd_dev, &response);
        return;
    }
    Status _status = {{}, 0, 0, 0};
    switch (message->protocolVersion)
    {
    case PROTOCOL_VERSION_1:
        switch (message->command)
        {
        case COMMAND_PING:
            memcpy(response.payload, message->payload, sizeof(response.payload));
            break;
        case COMMAND_HELLO:
            // XXX: los datos deberian ir al final del payload y no al comienzo...
            response.payload[0] = OPENEFI_VER_MAJOR;
            response.payload[1] = OPENEFI_VER_MINOR;
            response.payload[2] = OPENEFI_VER_REV;
            break;
        case COMMAND_STATUS:
            _status.RPM = sensors::values._MAP;
            _status.TEMP = sensors::values.TEMP;
            _status.V00 = sensors::values._TPS;
            memcpy(response.payload, &_status, 122);
            break;
        case COMMAND_BOOTL_SW:
            // Escribimos los bytes mágicos en los registros backup rtc
            // para forzar el arranque del bootloader
            rtc_backup_write(0, 0x544F4F42UL);
            scb_reset_system();
            return;
        default:
            response.command = COMMAND_ERR;
            response.subcommand = ERROR_INVALID_COMMAND;
        }
        break;

    default:
        // Protocolo invalido.
        response.command = COMMAND_ERR;
        response.subcommand = ERROR_INVALID_PROTOCOL;
    }

    send_message(usbd_dev, &response);
}
/** envia mensajes
 * @param usbd_dev puntero a constructor del USB
 * @param message Mensaje a enviar.
 */
void send_message(usbd_device *usbd_dev, SerialMessage *message)
{
    message->checksum = crc16((unsigned char *)message, 126);
    char *msg = (char *)message;
    usbd_ep_write_packet(usbd_dev, 0x82, msg, 64);
    for (int i = 0; i < 0x8000; i++)
        __asm__("nop");
    usbd_ep_write_packet(usbd_dev, 0x82, (msg + 64), 64);
}

bool get_frame(char *tempbuf, int len)
{
    if ((buffLength + len) <= 128)
    {
        memcpy(frameBuffer + buffLength, tempbuf, len);
        buffLength += len;
        if (buffLength >= 128)
        {
            return true;
        }
    }
    return false;
}

void usb_spam_loop()
{
}

uint16_t crc16(const unsigned char *data_p, uint8_t length)
{
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--)
    {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x << 5)) ^ ((uint16_t)x);
    }
    return crc;
}