/**
 * Library to control Sky box via USB 'sky controller'
 * 
 * Copyright (C) Joseph Heenan 2005-2007, all rights reserved.
 * Copyright (C) Ian Jeffray 2005, all rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR `AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * $Id: sky-control-lib.c,v 1.11 2009-07-06 15:40:43 joseph Exp $
 * 
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <usb.h>
#include <errno.h>

#include "sky-control-lib.h"

/* usb vendor and product id for sky controller */
#define VENDOR_ID  0x6666 
#define PRODUCT_ID 0xf100

struct sky_usb
{
    struct usb_device *device;
    usb_dev_handle    *handle;
    int                debug;
    int                timeout;
    int                interface_claimed;
};

int sky_usb_initialise(void)
{
    usb_set_debug(0);
    usb_init();
    usb_find_busses();
    usb_find_devices();
    
    return 0;
}

void sky_usb_finalise(void)
{
}

sky_usb *sky_usb_create(int debug, int timeout)
{
    sky_usb *sky = malloc(sizeof(*sky));
    if (sky == NULL)
        return NULL;
    
    sky->device            = NULL;
    sky->handle            = NULL;
    sky->debug             = debug;
    sky->timeout           = timeout;
    sky->interface_claimed = 0;

    return sky;
}

void sky_usb_destroy(sky_usb *sky)
{
    if (sky == NULL)
        return;

    if (sky->handle != NULL)
        usb_close(sky->handle);
    
    free(sky);
}

/**
 * Attempt to find the sky controller usb device
 * 
 * Scans through all the devices on all the usb busses and finds the
 * 'device_num'th controller.
 * 
 * @param device_num 0 for first device, 1 for second, etc
 */
int sky_usb_finddevice(sky_usb *sky, int device_num)
{
    int              device_count = 0;
    struct usb_bus  *bus;
    
    assert(sky != NULL);
    assert(sky->interface_claimed == 0);
    assert(device_num >= 0 && device_num < 255);
    
    for (bus = usb_get_busses(); bus != NULL; bus = bus->next)
    {
        struct usb_device *dev;

        for (dev = bus->devices; dev != NULL; dev = dev->next)
        {
            if (sky->debug)
            {
                printf("Found device: %04x-%04x\n",
                       dev->descriptor.idVendor,
                       dev->descriptor.idProduct);
            }
            if (dev->descriptor.idVendor  == VENDOR_ID &&
                dev->descriptor.idProduct == PRODUCT_ID)
            {
                if (sky->debug)
                    printf("Found sky control num %d\n", device_count);
                if (device_count == device_num)
                {
                    sky->device = dev;
                    sky->handle = usb_open(sky->device);
                    if (sky->handle == NULL)
                    {
                        perror("usb_open failed");
                        return -1;
                    }
                    
                    /* successfully opened device */
                    return 0;
                }
                device_count++;
            }
        }
    }
    
    fprintf(stderr, "sky_usb_finddevice(%d) failed to find device %04x-%04x\n",
            device_count, VENDOR_ID, PRODUCT_ID);
    
    return -1;
}

static int claim_interface(sky_usb *sky)
{
    int  ret;
    
    if (sky->interface_claimed == 0)
    {
#ifdef __linux__
        if (sky->debug)
            printf("Trying to detach kernel driver\n");
        
        /* try to detach device in case usbhid has got hold of it */
        ret = usb_detach_kernel_driver_np(sky->handle, 0);
        if (ret != 0)
        {
            if (errno == ENODATA)
            {
                if (sky->debug)
                    printf("Device already detached\n");
            }
            else
            {
                if (sky->debug)
                {
                    printf("Detach failed: %s[%d]\n", strerror(errno), errno);
                    printf("Continueing anyway\n");
                }
            }
        }
        else
        {
            if (sky->debug)
                printf("detach successful\n");
        }
#endif /* __linux__ */
        
        ret = usb_set_configuration(sky->handle, 1);
        if (ret < 0)
        {
            perror("usb_set_configuration failed");
            return -1;
        }
    
        ret = usb_claim_interface(sky->handle, 0);
        if (ret < 0)
        {
            perror("usb_claim_interface failed");
            return -1;
        }
        sky->interface_claimed = 1;
    }
    return 0;
}

static int getversion(sky_usb *sky)
{
    unsigned char buf[8];
    int  ret;
    
    assert(sky != NULL);
    
    ret = claim_interface(sky);
    if (ret != 0)
    {
        return -1;
    }
    
    memset(buf, 122, sizeof(buf));
    buf[0] = 'q';
    
    if (sky->debug)
    {
        printf("sending: byte %d\n", buf[0]);
    }
        
    ret = usb_interrupt_write(sky->handle, 1, (char *)buf, 8, sky->timeout);
    if (ret != 8)
    {
        perror("usb_interrupt_write failed");
        return -1;
    }
    
    ret = usb_interrupt_read(sky->handle, 1, (char *)buf, 8, sky->timeout);
    if (ret != 8)
    {
        perror("usb_interrupt_read failed");
        return -1;
    }
    
    printf("- Full version %d.%d.%d\n", buf[0], buf[1], buf[2]);
    
    return 0;
}

void sky_usb_printinfo(sky_usb *sky)
{
    char tmp_string[512];
    int  ret;
    
    assert(sky != NULL);
    assert(sky->interface_claimed == 0);
    
    if (sky->device->descriptor.iManufacturer)
    {
        ret = usb_get_string_simple(sky->handle,
                                    sky->device->descriptor.iManufacturer,
                                    tmp_string, sizeof(tmp_string));
        if (ret > 0)
          printf("- Manufacturer : %s\n", tmp_string);
        else
          printf("- Unable to fetch manufacturer string\n");
    }
    
    if (sky->device->descriptor.iProduct) 
    {
        ret = usb_get_string_simple(sky->handle,
                                    sky->device->descriptor.iProduct,
                                    tmp_string, sizeof(tmp_string));
        if (ret > 0)
          printf("- Product : %s\n", tmp_string);
        else
          printf("- Unable to fetch product string\n");
    }
    
                    
    if (sky->device->descriptor.iSerialNumber) 
    {
        ret = usb_get_string_simple(sky->handle,
                                    sky->device->descriptor.iSerialNumber,
                                    tmp_string, sizeof(tmp_string));
        if (ret > 0)
          printf("- Serial Number: %s\n", tmp_string);
        else
          printf("- Unable to fetch serial number string\n");
    }
    
    printf("- Version Number: %d.%d\n",
           sky->device->descriptor.bcdDevice / 256,
           sky->device->descriptor.bcdDevice % 256);
    
    (void) getversion(sky);
}

int sky_usb_sendcommand(sky_usb *sky, int output, int command)
{
    unsigned char buf[8];
    int  ret;
    
    assert(sky != NULL);
    assert(output >= 0);
    assert(output < 15);
    assert(command >= 0 && command < 0xffff);
    
    ret = claim_interface(sky);
    if (ret != 0)
    {
        return -1;
    }
    
    memset(buf, 122, sizeof(buf));
    if (output <= 1)
    {
        buf[0] = output == 0 ? '0' : '1';
        buf[1] = command >> 8;
        buf[2] = command & 0xff;
    }
    else
    {
        /* more than two outputs, use 4 byte control sequence */
        buf[0] = 'C';
        buf[1] = output;
        buf[2] = command >> 8;
        buf[3] = command & 0xff;
    }
    
    if (sky->debug)
    {
        printf("sending cmd %04x to output %d: bytes %d, %d, %d\n",
               command, output+1, buf[0], buf[1], buf[2]);
    }
        
    ret = usb_interrupt_write(sky->handle, 1, (char *)buf, 8, sky->timeout);
    if (ret != 8)
    {
        perror("usb_interrupt_write failed");
        return -1;
    }
    
    return 0;
}

int sky_usb_sendconfig(sky_usb *sky, int output, sky_config *config)
{
    unsigned char buf[8];
    int  ret;
    
    assert(sky != NULL);
    assert(output == 0 || output == 1);
    assert(config != NULL);
    
    ret = claim_interface(sky);
    if (ret != 0)
    {
        return -1;
    }
    
    memset(buf, 122, sizeof(buf));
    buf[0] = output == 0 ? 's' : 't';
    buf[1] = config->repeatdelay;
    buf[2] = config->interkeydelay;
    buf[3] = config->samekeydelay;
    buf[4] = config->interkeydelayhigh;
    buf[5] = config->interkeydelaylow;
    buf[6] = config->interbitdelayhigh;
    buf[7] = config->interbitdelaylow;
    
    ret = usb_interrupt_write(sky->handle, 1, (char *)buf, 8, sky->timeout);
    if (ret != 8)
    {
        perror("usb_interrupt_write failed");
        return -1;
    }

    memset(buf, 122, sizeof(buf));
    buf[0] = output == 0 ? 'u' : 'v';
    buf[1] = config->repeatcount;

    ret = usb_interrupt_write(sky->handle, 1, (char *)buf, 8, sky->timeout);
    if (ret != 8)
    {
        perror("usb_interrupt_write failed");
        return -1;
    }
    
    return 0;
}
