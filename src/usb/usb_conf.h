#ifndef USB_CONF_H_INCLUDED
#define USB_CONF_H_INCLUDED

#include <libopencm3/usb/usbd.h>

#define USB_VID                 0x1209
#define USB_PID                 0xEEF1
#define USB_CONTROL_BUF_SIZE    1024
#define USB_SERIAL_NUM_LENGTH   24
#define INTF_DFU                0

extern usbd_device* usb_setup(void);
const usbd_driver* usb_init(void);
#endif