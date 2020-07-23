/*
 * Copyright (c) 2016, Devan Lai
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice
 * appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <libopencm3/usb/dfu.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/stm32/desig.h>
#include "webusb.h"

#include "usb21.h"
#include "webusb.h"
#include "usb_conf.h"

/* Table 6: Data Interface Class Code */
#define USB_CLASS_DATA			0x0A

// Endpoints disponibles
static const struct usb_endpoint_descriptor data_endp[2] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
}};

// Interfaz principal
static const struct usb_interface_descriptor data_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 1,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_DATA,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = data_endp,
};

// Listado de todas las interfaces
static const struct usb_interface interfaces[] = {
    {
        .num_altsetting = 1,
        .altsetting = &data_iface,
    }
};

// Dispositivo
static const struct usb_device_descriptor dev = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    // USB 1.1
    .bcdUSB = 0x0210,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    // Versión del dispositivo (2.0.0)
    .bcdDevice = 0x0200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

static const struct usb_config_descriptor config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = sizeof(interfaces)/sizeof(struct usb_interface),
    .bConfigurationValue = 1,
    .iConfiguration = 0x04,
    .bmAttributes = 0xC0,
    .bMaxPower = 0x32,

    .interface = interfaces,
};

static const struct usb_device_capability_descriptor* capabilities[] = {
    (const struct usb_device_capability_descriptor*)&webusb_platform,
    // TODO: WinUSB?
};

static const struct usb_bos_descriptor bos_descriptor = {
    .bLength = USB_DT_BOS_SIZE,
    .bDescriptorType = USB_DT_BOS,
    .bNumDeviceCaps = sizeof(capabilities)/sizeof(capabilities[0]),
    .capabilities = capabilities
};

static char serial_number[USB_SERIAL_NUM_LENGTH+1]; 
static const char *usb_strings[] = {
    "OpenEFI",                      // iManufacturer
    "OpenEFI v2",                   // iProduct
    serial_number,                  // iSerialNumber
    "OpenEFI Control Interface"     // iConfiguration(1)
};

/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[USB_CONTROL_BUF_SIZE] __attribute__ ((aligned (2)));

const usbd_driver* usb_init() {
    return &st_usbfs_v1_usb_driver;
}

usbd_device* usb_setup() {
    int num_strings = sizeof(usb_strings)/sizeof(const char*);
    desig_get_unique_id_as_string(serial_number, USB_SERIAL_NUM_LENGTH+1);
    serial_number[USB_SERIAL_NUM_LENGTH] = '\0';

    usbd_device* usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config,
                                      usb_strings, num_strings,
                                      usbd_control_buffer, sizeof(usbd_control_buffer));
    usb21_setup(usbd_dev, &bos_descriptor);
    return usbd_dev;
}