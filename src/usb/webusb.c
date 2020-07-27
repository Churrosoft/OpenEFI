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
#include "control_interface.h"
#include <string.h>
#include <libopencm3/usb/usbd.h>
#include "webusb.h"
#include <stddef.h>
#include "usb_conf.h"

#ifndef LANDING_PAGE_URL
#define LANDING_PAGE_URL "tuner.openefi.xyz/"
#define USB_DT_DEVICE_CAPABILITY 16
#define USB_DC_PLATFORM 5
#endif

#define LANDING_PAGE_DESCRIPTOR_SIZE (WEBUSB_DT_URL_DESCRIPTOR_SIZE + sizeof(LANDING_PAGE_URL) - 1)

_Static_assert((LANDING_PAGE_DESCRIPTOR_SIZE < 256),
               "Landing page URL is too long");

const struct webusb_platform_descriptor webusb_platform = {
    .bLength = WEBUSB_PLATFORM_DESCRIPTOR_SIZE,
    .bDescriptorType = USB_DT_DEVICE_CAPABILITY,
    .bDevCapabilityType = USB_DC_PLATFORM,
    .bReserved = 0,
    .platformCapabilityUUID = WEBUSB_UUID,
    .bcdVersion = 0x0100,
    .bVendorCode = WEBUSB_VENDOR_CODE,
    .iLandingPage = 1};

static const struct webusb_url_descriptor landing_url_descriptor = {
    LANDING_PAGE_DESCRIPTOR_SIZE,
    WEBUSB_DT_URL,
    WEBUSB_URL_SCHEME_HTTPS,
    LANDING_PAGE_URL};

static enum usbd_request_return_codes
webusb_control_vendor_request(
    usbd_device *usbd_dev,
    struct usb_setup_data *req,
    uint8_t **buf, uint16_t *len,
    usbd_control_complete_callback *complete)
{
    (void)complete;
    (void)usbd_dev;

    if (req->bRequest != WEBUSB_VENDOR_CODE)
        return USBD_REQ_NEXT_CALLBACK;

    switch (req->wIndex)
    {
    case WEBUSB_REQ_GET_URL:
        if (req->wValue != 1)
            break;
        *buf = (uint8_t *)(&landing_url_descriptor);
        *len = landing_url_descriptor.bLength;
        return USBD_REQ_HANDLED;
        break;
    default:
        return USBD_REQ_NOTSUPP;
        break;
    }
    return USBD_REQ_NOTSUPP;
}

// Recepción de datos
static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
    (void)ep;
    (void)usbd_dev;

    char tempbuf2[64] = "";

    int len = usbd_ep_read_packet(usbd_dev, 0x01, tempbuf2, 64);
    if (len > 0)
    {
        if (get_frame(tempbuf2, len))
        {
            // Cuando ya tenemos un frame completo, lo procesamos.
            process_frame(usbd_dev, (SerialMessage *)&frameBuffer);

            memset(frameBuffer, 0, buffLength);
            buffLength = 0;
        }
        memset(tempbuf2, 0, len);
        len = 0;
    }
}

static void webusb_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
    (void)wValue;
    usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
    usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
    usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);
    usbd_register_control_callback(
        usbd_dev,
        USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        webusb_control_vendor_request);
}

void webusb_setup(usbd_device *usbd_dev)
{
    usbd_register_set_config_callback(usbd_dev, webusb_set_config);
}
