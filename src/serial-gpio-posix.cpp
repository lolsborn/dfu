/*
  Copyright (C) 2019 Ronoth, LLC <steven@ronoth.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <stdio.h>
#include <serial-gpio.h>
#include "sleepms.h"
#include "libusb.h"

struct libusb_context *usb_ctx;
static libusb_device_handle *cp210x_handle;

#define PID 0xea60
#define VID 0x10c4

// Functions borrowed from libv/flashrom
// Copyright (C) 2018 Linaro Limited
static int cp210x_gpio_get(void)
{
    int res;
    uint8_t gpio;

    res = libusb_control_transfer(cp210x_handle, REQTYPE_DEVICE_TO_HOST,
                                  CP210X_VENDOR_SPECIFIC, CP210X_READ_LATCH,
                                  0, &gpio, 1, 0);
    if (res < 0) {
        printf("Failed to read GPIO pins (%s)\n", libusb_error_name(res));
        return 0;
    }

    return gpio;
}

static void cp210x_gpio_set(uint8_t val, uint8_t mask)
{
    int res;
    uint16_t gpio;

    gpio = ((val & 0xf) << 8) | (mask & 0xf);

    /* Set relay state on the card */
    res = libusb_control_transfer(cp210x_handle, REQTYPE_HOST_TO_DEVICE,
                                  CP210X_VENDOR_SPECIFIC, CP210X_WRITE_LATCH,
                                  gpio, NULL, 0, 0);
    if (res < 0)
        printf("Failed to set GPIO pins (%s)\n", libusb_error_name(res));
}


void printDevInfo(serial* device) {
}


void toggleBootFinish(serial *device) {
    sleep_ms(100);
}

void toggleBootStart(serial *device) {
    // Initialize libusb
    int r = libusb_init(&usb_ctx); //initialize the library for the session we just declared
    libusb_set_option(usb_ctx, LIBUSB_OPTION_LOG_LEVEL, 3);
    if(r < 0) {
        printf("LibUSB: error with init\n");
        return 1;
    }
    printf("opening 0x%04x:0x%04x\n", VID, PID);
    cp210x_handle  = libusb_open_device_with_vid_pid(usb_ctx, VID, PID);
    if(cp210x_handle == NULL) {
        printf("device not found\n");
        return 1;
    }

    cp210x_gpio_set(0xff, 0x00ff);
    sleep_ms(100);
    cp210x_gpio_set(BOOT0, BOOT0);
    sleep_ms(100);
    cp210x_gpio_set(BOOT0, NRST);
    sleep_ms(100);
}
