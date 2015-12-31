/*
 * $RCSfile: ikd.c,v $  $Revision: 1.1 $  $Name:  $
 * $Id: ikd.c,v 1.1 2006/10/04 03:34:14 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2006/10/04 03:34:14 $
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
 *  Device dependent Implimentation code for communications with Neil
 *  Cherry's usb driver for Insteon PLC's. For this to work, the usb driver
 *  must be loaded.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#include "idrv.h"

int usb_cts_sleep = 120; /* usecs to sleep between USB reads */
int usb_send_to = 35;

void *ikd_init(char *dev);
void ikd_close(iusb_t *iplc);
int ikd_send_pkt(iusb_t *iplc, unsigned char *pkt, int rt);
int ikd_send(iusb_t *iplc, unsigned char *pkt, int len);
int ikd_cts(iusb_t *iplc, int cts);
int ikd_read_byte(iusb_t *iplc);


/*
 * ikd_init
 *
 * This function initializes the USB connection to the PLC. This must
 * be called before any other function since the handle returned is
 * passed as an argument to all other functions.
 */
void *ikd_init(char *dev)
{
	int *fd;

	fd = (int *)malloc(sizeof(int));

	*fd = open (dev, O_NONBLOCK | O_RDWR);

	if (*fd < 0) {
		fprintf(stderr, "Error opening iplc device %s\n", dev);
		return NULL;
	}

	return (void *)fd;
}


/*
 * ikd_close
 *
 * Release control of the PLC and shutdown the connection to it.
 */
void ikd_close(iusb_t *iplc)
{
	int *fd;

	/* Cast the interface descriptor into the proper type (file descriptor) */
	fd = (int *)iplc->vfd;

	/* Close the file descriptor */
	close(*fd);
}


/*
 * ikd_read_byte     -- Read packets from the USB interface and buffer.
 * ikd_cts           -- Wait for a clear-to-send from the USB interface.
 * ikd_send_pkt      -- Write a data packet to the USB interface
 *
 */

/*
 * ikd_read_byte
 *
 * This reads packets from the USB driver interface and returns 1 byte at
 * a time to the caller.
 *
 * Parameters:
 *   iusb_t interface handle
 *
 * Returns:
 *   success: byte read
 *   error:   -110
 */

#define URB_TIMEOUT 60 /* This is near the minimum, might have to increase it */
int ikd_read_byte(iusb_t *iplc)
{
	unsigned char packet_buf[8];
	int cnt = 0;
	int ret;
	int i;
	int timeout = 0;
	unsigned char rb;
	int *fd;

	fd = (int *)iplc->vfd;
	/* Wrap ring buffer if necessary */
	if (iplc->ring_head == RING_SIZE) {
		iplc->ring_head = 0;
	}

	/* Check for empty ring buffer */
	if (iplc->ring_head == iplc->ring_tail) {  /* ring buffer empty */
		do {
			memset(packet_buf, 0, 8);
			ret = read(*fd, packet_buf, 8);
			if (ret == 8) {
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
			} else if (ret > 0) {
				printf("Got %d bytes instead of 8!\n", ret);
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
 * ikd_cts
 *
 * Device dependent function that waits for the PCL to provide a CTS signal.
 *
 * Parameters:
 *   iusb_t interface handle
 *   cts    If >0 then only loop once.
 *
 * We could end up locking up in this function.
 */
int ikd_cts(iusb_t *iplc, int cts)
{
	unsigned char usbp[8];
	int i;
	int ret;
	int timeout = 0;
	int *fd;

	fd = (int *)iplc->vfd;

	do {
		memset(usbp, 0, 8);
		ret = read(*fd, usbp, 8);
		if (ret == 8) {
			if (usbp[0] & 0x0f) { /* need to add to ring buffer */
				for (i = 0; i < (usbp[0] & 0x0f); i++) {
					iplc->ring_buffer[iplc->ring_tail++] = usbp[i+1];
					if (iplc->ring_tail == RING_SIZE) {
						iplc->ring_tail = 0;
					}
				}
			}
			if (usbp[0] & 0x70) {
				printf("CTS: got something in high nybble 0x%02x\n", usbp[0]);
			}
		} else if (ret > 0) {
			printf("Got %d bytes instead of 8!\n", ret);
		}
		timeout++;
		usleep(usb_cts_sleep);
	} while ((!(usbp[0] & 0x80)) && (timeout < 5500) && !cts);

	if (timeout >= 5500) {
		fprintf(stderr, "Timeout in ikd_cts (%d)\n", cts);
	}
	return 0;
}


/*
 * ikd_send_pkt
 *
 * Device dependent function to send a two byte packet and wait for the
 * PLC echo to show up in the ring buffer.
 *
 * Parameters:
 *   iplc   interface handle.
 *   pkt    character buffer holding the characters to send.
 *   len    size of the expected response?
 *
 */
int ikd_send_pkt(iusb_t *iplc, unsigned char *pkt, int len)
{
	int ret;
	int rs;
	int to; /* number of loops before timing out */
	int *fd;

	fd = (int *)iplc->vfd;

	to = usb_send_to;
	rs = iplc->ring_tail;
	ret = write(*fd, pkt, 8);
	ikd_cts(iplc, 0);

	/* Got a clear to send, but did the bytes get echoed back? */
	while (to) {
		if (rb_avail(iplc, rs, len) < 2) {
			ikd_cts(iplc, 1);
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

	fprintf(stderr, "Timeout waiting for PLC to echo data.\n");
	return ret;
}

/*
 * ikd_send
 *
 * Send bytes to the interface.  This function does not wait for the PLC
 * to echo back the data sent.
 *
 * Parameters:
 *   iplc     interface handle
 *   pkt      buffer with data to send
 *   len      number of bytes to send
 */
int ikd_send(iusb_t *iplc, unsigned char *pkt, int len)
{
	int ret;
	int *fd;

	fd = (int *)iplc->vfd;

	ret = write(*fd, pkt, len);

	return ret;
}
