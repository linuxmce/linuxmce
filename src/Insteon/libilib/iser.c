/*
 * $RCSfile: iser.c,v $  $Revision: 1.2 $  $Name:  $
 * $Id: iser.c,v 1.2 2007/06/29 02:16:23 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/06/29 02:16:23 $
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
 *  Device dependent Implimentation code for communications with a serial
 *  Insteon PLC.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <termios.h>

#include "idrv.h"

void *iser_init(char *dev);
void iser_close(iusb_t *iplc);
int iser_read(iusb_t *iplc);
int iser_write(iusb_t *iplc, unsigned char *pkt, int len);
int iser_write_echo(iusb_t *iplc, unsigned char *pkt, int rt);
int iser_cts(iusb_t *iplc, int cts);

static struct termios saved_io;

/*
 * iser_init
 *
 * This function initializes the USB connection to the PLC. This must
 * be called before any other function since the handle returned is
 * passed as an argument to all other functions.
 */
void *iser_init(char *dev)
{
	int *fd;
	struct termios io;

	fd = (int *)malloc(sizeof(int));

	*fd = open (dev, O_RDWR | O_NOCTTY);

	if (*fd < 0) {
		fprintf(stderr, "Error opening iplc device %s\n", dev);
		return NULL;
	}

	/* Save current serial port settings */
	tcgetattr(*fd, &saved_io);

	/*
	 * Set up serial port for communication with PLC
	 *    4800 baud 8bit no parity
	 */
	io.c_cflag = B4800 | CS8 | CREAD;
	io.c_iflag = IGNPAR | IGNBRK;
	io.c_oflag = 0;
	io.c_lflag = 0;

	tcsetattr(*fd, TCSANOW, &io);

	return (void *)fd;
}


/*
 * iser_close
 *
 * Release control of the PLC and shutdown the connection to it.
 */
void iser_close(iusb_t *iplc)
{
	int *fd;

	/* Cast the interface descriptor into the proper type (file descriptor) */
	fd = (int *)iplc->vfd;

	/* Restore the serial port settings */
	tcsetattr(*fd, TCSANOW, &saved_io);

	/* Close the file descriptor */
	close(*fd);
}


/*
 * iser_read       -- Read packets from the USB interface and buffer.
 * iser_write      -- Write a data packet to the USB interface
 * iser_write_echo -- Write a data packet to the USB interface
 * iser_cts        -- Wait for a clear-to-send from the USB interface.
 *
 */

/*
 * iser_read
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
int iser_read(iusb_t *iplc)
{
	unsigned char packet_buf[8];
	int cnt = 0;
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
			cnt = read(*fd, packet_buf, 8);
			for (i = 0; i < cnt; i++) {
				iplc->ring_buffer[iplc->ring_tail++] = packet_buf[i];
				if (iplc->ring_tail == RING_SIZE) {
					iplc->ring_tail = 0;
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
 * iser_cts
 *
 * Device dependent function that waits for the PCL to provide a CTS signal.
 *
 * Parameters:
 *   iusb_t interface handle
 *   cts    If >0 then only loop once.
 *
 * We could end up locking up in this function.
 */
int iser_cts(iusb_t *iplc, int cts)
{
	int *fd;
	unsigned char pkt;
	int ret;

	fd = (int *)iplc->vfd;

	ret = read(*fd, &pkt, 1);
	iplc->ring_buffer[iplc->ring_tail++] = pkt;
	if (iplc->ring_tail == RING_SIZE) {
		iplc->ring_tail = 0;
	}

	return 0;
}


/*
 * iser_write_echo
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
int iser_write_echo(iusb_t *iplc, unsigned char *pkt, int len)
{
	int ret;
	int rs;
	int to = 100; /* number of loops before timing out */
	int *fd;

	fd = (int *)iplc->vfd;

	rs = iplc->ring_tail;
	ret = write(*fd, pkt, 2);
	iser_cts(iplc, 0);

	/* Got a clear to send, but did the bytes get echoed back? */
	while (to) {
		if (rb_avail(iplc, rs, len) < 2) {
			iser_cts(iplc, 1);
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
 * iser_write
 *
 * Send bytes to the interface.  This function does not wait for the PLC
 * to echo back the data sent.
 *
 * Parameters:
 *   iplc     interface handle
 *   pkt      buffer with data to send
 *   len      number of bytes to send
 */
int iser_write(iusb_t *iplc, unsigned char *pkt, int len)
{
	int ret;
	int *fd;

	fd = (int *)iplc->vfd;

	ret = write(*fd, pkt, len);

	return ret;
}
