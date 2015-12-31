/*
 * $RCSfile: ihidlib.c,v $  $Revision: 1.1 $  $Name:  $
 * $Id: ihidlib.c,v 1.1 2007/04/21 03:23:27 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/04/21 03:23:27 $
 * ----------------------------------------------------------------------------
 *
 *  Copyright (c) Bob Paauwe (2006)
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * ----------------------------------------------------------------------------
 *
 *  Description:
 *
 *  This is the interface dependent code that use libhid communicate with
 *  the Insteon PLC.  This requires libhid (http://libhid.alioth.debian.org/)
 *  to be installed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#ifdef INCLUDE_LIBHID

#include <hidlib.h>

#include "idrv.h"

void *ihidlib_init(char *dev);
void ihidlib_close(iusb_t *iplc);
int ihidlib_read(iusb_t *iplc);
int ihidlib_write(iusb_t *iplc, unsigned char *pkt, int len);
int ihidlib_write_echo(iusb_t *iplc, unsigned char *pkt, int rt);
int ihidlib_cts(iusb_t *iplc, int cts);


/*
 * ihidlib_init
 *
 * This function initializes the USB connection to the PLC. This must
 * be called before any other function since the handle returned is
 * passed as an argument to all other functions.
 */
void *ihidlib_init(char *dev)
{
	HIDInterface *ud = NULL;
	HIDInterfaceMatcher plc_match = {IPLC_VENDOR, IPLC_PRODUCT, NULL, NULL, 0};

	/*
	hid_set_debug(HID_DEBUG_WARNINGS);
	hid_set_debug_stream(stderr);
	*/
	hid_set_usb_debug(0);

	if (hid_init() != HID_RET_SUCCESS) {
		fprintf(stderr, "Failed to initialize HID interface\n");
		return NULL;
	}

	ud = hid_new_HIDInterface();
	if (ud == 0) {
		fprintf(stderr, "hid_new_HIDInterface() failed.\n");
		return NULL;
	}

	/* Try 3 times to detach the PLC from the HID interface */
	if (hid_force_open(ud, 0, &plc_match, 3) != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_force_open() failed.\n");
		return NULL;
	}

	return (void *)ud;
}


/*
 * ihidlib_close
 *
 * Release control of the PLC and shutdown the connection to it.
 */
void ihidlib_close(iusb_t *iplc)
{
	HIDInterface *ud = (HIDInterface *)iplc->vfd;

	hid_close(ud);
	hid_delete_HIDInterface(&(ud));
	hid_cleanup();
}

/*
 * Low level HID/PLC commands
 *
 * This section has the functions that work at a fairly low level when
 * controlling the USB plc. These functions should probably only be
 * called by the higher level functions and not by the top level
 * application.
 *
 * The functions are:
 *
 * ihidlib_read       -- Read packets from the USB interface and buffer.
 * ihidlib_write      -- Write a data packet to the USB interface
 * ihidlib_write_echo -- Write a data packet to the USB interface
 * ihidlib_cts        -- Wait for a clear-to-send from the USB interface.
 *
 */


/*
 * ihidlib_read
 *
 * This reads packets from the USB interface and returns 1 byte at
 * a time to the caller.
 */

#define URB_TIMEOUT 5 /* This is near the minimum, might have to increase it */
int ihidlib_read(iusb_t *iplc)
{
	HIDInterface *ud = (HIDInterface *)iplc->vfd;
	unsigned char packet_buf[8];
	int cnt = 0;
	int ret;
	int i;
	int timeout = 0;
	unsigned char rb;

	if (iplc->ring_head == RING_SIZE) {
		iplc->ring_head = 0;
	}

	if (iplc->ring_head == iplc->ring_tail) {  /* ring buffer empty */
		do {
			memset(packet_buf, 0, 8);
			ret = hid_interrupt_read(ud, USB_ENDPOINT_IN+1,
					(char *)packet_buf, 8, 100);
			if (ret == HID_RET_SUCCESS) {
				cnt = packet_buf[0] & 0x0f;
				for (i = 0; i < cnt; i++) {
					iplc->ring_buffer[iplc->ring_tail++] = packet_buf[i+1];
					if (iplc->ring_tail == RING_SIZE) {
						iplc->ring_tail = 0;
					}
				}
				if (packet_buf[0] & 0x70) {
					printf("got something in high nybble 0x%02x\n",
							packet_buf[0]);
				}
			}
			timeout++;
			usleep(100);
		} while ((cnt < 1) && (timeout < URB_TIMEOUT));
	}

	if (iplc->ring_head != iplc->ring_tail) {
		/* When poping a byte from the ring buffer, clear it */
		rb = iplc->ring_buffer[iplc->ring_head];
		iplc->ring_buffer[iplc->ring_head] = 0x00;
		iplc->ring_head++;
		return rb;
	} else {
		return -110;
	}
}

/*
 * ihidlib_cts
 *
 * Wait for the PCL to give us a CTS signal.  We could end up locking
 * up in this function.
 */
int ihidlib_cts(iusb_t *iplc, int cts)
{
#if 0
	HIDInterface *ud = (HIDInterface *)iplc->vfd;
	unsigned char usbp[8];
	int i;
	int ret;
	int timeout = 0;

	do {
		/* usbp[0] = 0; */
		memset(usbp, 0, 8);
		/* ret = read(iplc->fd, usbp, 8); */
		/* ret = usb_interrupt_read(iplc->ud, 1, (char *)usbp, 8, 500); */
		ret = hid_interrupt_read(iplc->ud, USB_ENDPOINT_IN+1,
				(char *)usbp, 8, 1000);
		if (ret == HID_RET_SUCCESS) {
			if (usbp[0] & 0x0f) { /* need to add to ring buffer */
				for (i = 0; i < (usbp[0] & 0x0f); i++) {
					ring_buffer[ring_tail++] = usbp[i+1];
					if (ring_tail == RING_SIZE) {
						ring_tail = 0;
					}
				}
			}
			if (usbp[0] & 0x70) {
				printf("CTS: got something in high nybble 0x%02x\n", usbp[0]);
			}
		} else if (ret > 0) {
			printf("usb_cts: HID read returned %d\n", ret);
		}
		timeout++;
		usleep(120);
	} while ((!(usbp[0] & 0x80)) && (timeout < 2) && !cts);

	if (timeout >= 2) {
		printf("Timeout in usb_cts (%d)\n", cts);
	}
#endif
	return 0;
}


/*
 * ihidlib_write_echo
 *
 * Send a two byte packet and wait for the ring buffer tail to reach rt.
 */
int ihidlib_write_echo(iusb_t *iplc, unsigned char *pkt, int len)
{
	HIDInterface *ud = (HIDInterface *)iplc->vfd;
	int ret;
	int rs;
	int to; /* number of loops before timing out */

	to = 35;
	to = 1;
	rs = iplc->ring_tail;
	ret = hid_interrupt_write(ud, USB_ENDPOINT_OUT+1, (char *)pkt, 8, 10);
	ihidlib_cts(iplc, 0);

	/* Got a clear to send, but did the bytes get echoed back? */
#if 0
	while (to) {
		if (rb_avail(iplc, rs, len) < 2) {
			ihidlib_cts(iplc, 1);
			to--;
		} else {
			print_ring(iplc);
			while (rb_avail(iplc, rs, len) >= 2) {
				if (echo_match(iplc, rs, &pkt[1], len)) {
					return ret;
				}
				if (rs == iplc->ring_head) {
					iplc->ring_buffer[iplc->ring_head++] = 0x00;
				}
				if (++rs == RING_SIZE) {
					rs = 0;
				}
			}
		}
	}

	printf("Timeout trying to get echo back from PLC\n");
#endif
	return ret;
}

/*
 * ihidlib_send
 *
 * Send bytes to the interface.  This function does not wait for the PLC
 * to echo back the data sent.
 *
 * Parameters:
 *   iplc     interface handle
 *   pkt      buffer with data to send
 *   len      number of bytes to s
 */
int ihidlib_write(iusb_t *iplc, unsigned char *pkt, int len)
{
	HIDInterface *ud = (HIDInterface *)iplc->vfd;
	int ret;

	ret = hid_interrupt_write(ud, USB_ENDPOINT_OUT+1, (char *)pkt, len, 10);

	return ret;
}
#endif
