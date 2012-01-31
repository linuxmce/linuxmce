/**
 * Library to control Sky box via USB 'sky controller'
 *
 * Copyright (C) Joseph Heenan 2005-2007, all rights reserved.
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
 * $Id: sky-control-lib.h,v 1.3 2009-07-06 15:40:45 joseph Exp $
 *
 */

#ifndef SKY_CONTROL_LIB_H
#define SKY_CONTROL_LIB_H

#ifdef __cplusplus
extern "C" {
#endif
/** opaque handle for device */
typedef struct sky_usb sky_usb;

typedef struct sky_config
{
    /** set to 150 - decrease to speed up sending of non-identical digits */
    int interkeydelay;

    /** set to 150 - decrease to speed up sending of identical digits */
    int samekeydelay;

    int interkeydelayhigh; /* set to 0x05 */
    int interkeydelaylow;  /* set to 0xdc */
    int interbitdelayhigh; /* set to 0x0a */
    int interbitdelaylow; /* set to 0x8b */
    int repeatcount; /* set to 1 */
    int repeatdelay; /* set to 1 */
}
sky_config;

/**
 * Initialise library
 *
 * Must be called before calling any other functions
 *
 * @return 0 on success, -1 on failure
 */
int sky_usb_initialise(void);

/**
 * Finalise library
 *
 * Do not call any library functions after calling this.
 */
void sky_usb_finalise(void);

/**
 * Create a device handle
 *
 * @param debug   set to 1 to enable debug, 0 otherwise
 * @param timeout usb comms timeout in milliseconds (1000 would be reasonable)
 *
 * @return pointer to opaque device handle on success, NULL on failure
 */
sky_usb *sky_usb_create(int debug, int timeout);

/**
 * Destroy a device handle
 *
 * @param sky device handle to destroy
 */
void sky_usb_destroy(sky_usb *sky);

/**
 * Attempt to find the sky controller usb device
 *
 * Scans through all the devices on all the usb busses and finds the
 * 'device_num'th controller.
 *
 * @param sky        library handle
 * @param device_num 0 for first device, 1 for second, etc
 *
 * @return 0 on success, -1 on failure
 */
int sky_usb_finddevice(sky_usb *sky, int device_num);

/**
 * Print out device info
 *
 * Prints manufacturer, product and serial number, if available.
 *
 * @param sky        library handle
 */
void sky_usb_printinfo(sky_usb *sky);

/**
 * Send a usb command
 *
 * @param sky        library handle
 * @param output     output number (numbered from 0)
 * @param command    16-bit command (eg. 0x0C01 to send '1' to sky+ box)
 *
 * @return 0 on success, -1 on failure
 */
int sky_usb_sendcommand(sky_usb *sky, int output, int command);

/**
 * Send a usb configuration command
 *
 * NB. Configuration is temporary - it is lost when the controller loses
 * power.
 *
 * @param sky        library handle
 * @param output     output number (0 or 1)
 * @param config     pointer to sky controller configuration structure
 *
 * @return 0 on success, -1 on failure
 */
int sky_usb_sendconfig(sky_usb *sky, int output, sky_config *config);

#ifdef __cplusplus
}
#endif

#endif /* SKY_CONTROL_LIB_H */
