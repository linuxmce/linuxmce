/*
 * $RCSfile: ihid.c,v $  $Revision: 1.5 $  $Name:  $
 * $Id: ihid.c,v 1.5 2007/09/09 18:09:45 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/09/09 18:09:45 $
 * ----------------------------------------------------------------------------
 *
 *  Copyright (c) Bob Paauwe (2007)
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
 *  Device dependent Implimentation code for communications with an USB
 *  Insteon PLC via the HIDDEV interface.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/hiddev.h>

#include "idrv.h"

int hid_cts_sleep = 120; /* usecs to sleep between USB reads */

void *ihid_init(char *dev);
void ihid_close(iusb_t *iplc);
int ihid_read(iusb_t *iplc);
int ihid_write(iusb_t *iplc, unsigned char *pkt, int len);
int ihid_write_echo(iusb_t *iplc, unsigned char *pkt, int rt);
int ihid_cts(iusb_t *iplc, int cts);

static int ihid_read_usb (int fd, unsigned char *pkt, int *count);

/*
 * ihid_init
 *
 * This function initializes the USB connection to the PLC. This must
 * be called before any other function since the handle returned is
 * passed as an argument to all other functions.
 */
void *ihid_init(char *dev)
{
	int *fd;
	struct hiddev_devinfo info;

	fd = (int *)malloc(sizeof(int));

	*fd = open (dev, O_NONBLOCK | O_RDWR);

	if (*fd < 0) {
		fprintf(stderr, "Error opening hiddev device %s\n", dev);
		free(fd);
		return NULL;
	}

	/*
	 * Verify that this is a PLC by checking the vendor and product
	 * Vendor should be 0x1bf
	 * Product should be 4 (are the more product id's?
	 */
	ioctl(*fd, HIDIOCGDEVINFO, &info);
	if (info.vendor != 0x10bf) { /* Smarthome */
		fprintf(stderr, "Vendor is not Smarthome, so probably not a PLC\n");
		close (*fd);
		free(fd);
		return NULL;
	}

	return (void *)fd;
}


/*
 * ihid_close
 *
 * Release control of the PLC and shutdown the connection to it.
 */
void ihid_close(iusb_t *iplc)
{
	int *fd;

	/* Cast the interface descriptor into the proper type (file descriptor) */
	fd = (int *)iplc->vfd;

	/* Close the file descriptor */
	close(*fd);
	free(fd);
}


/*
 * ihid_read       -- Read packets from the USB interface and buffer.
 * ihid_write      -- Write a data packet to the USB interface
 * ihid_write_echo -- Write a data packet to the USB interface
 * ihid_cts        -- Wait for a clear-to-send from the USB interface.
 *
 */

/*
 * ihid_read
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
 *
 * This might seem overly complex, but reads from the hid device are always
 * 8 byte packets, but the packet may have between 0 and 7 bytes of data
 * in it.  By using a ring-buffer concept, this doesn't need to keep track
 * of all the state information to track variable length data in the 8
 * byte packet.
 */

#define URB_TIMEOUT 60 /* This is near the minimum, might have to increase it */

int ihid_read(iusb_t *iplc)
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
			ihid_read_usb(*fd, packet_buf, &cnt);
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
 * ihid_cts
 *
 * Device dependent function that waits for the PCL to provide a CTS signal.
 *
 * Parameters:
 *   iusb_t interface handle
 *   once    If >0 then only loop once.
 *
 * We could end up locking up in this function.
 */
int ihid_cts(iusb_t *iplc, int once)
{
	unsigned char usbp[8];
	int i;
	int cnt;
	int cts = 0;
	int timeout = 0;
	int *fd;

	fd = (int *)iplc->vfd;

	do {
		memset(usbp, 0, 8);
		cnt = 0;
		cts = ihid_read_usb(*fd, usbp, &cnt);

		/* If count is > 0, then there is data in this packet */
		if (cnt) {
			for (i = 0; i < cnt; i++) {
				iplc->ring_buffer[iplc->ring_tail++] = usbp[i];
				if (iplc->ring_tail == RING_SIZE) {
					iplc->ring_tail = 0;
				}
			}
		}

		if (once) {
			return 0;
		} else {
			timeout++;
			usleep(hid_cts_sleep);
		}
	} while ((timeout < 5500) && (!cts));

	if (timeout >= 5500) {
		fprintf(stderr, "Timeout in ihid_cts (%d)\n", cts);
	}
	return 0;
}


/*
 * ihid_write_echo
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
int ihid_write_echo(iusb_t *iplc, unsigned char *pkt, int len)
{
	int ret;
	int rs;
	int to = 100; /* number of loops before timing out */
	int *fd;

	fd = (int *)iplc->vfd;

	rs = iplc->ring_tail;
	ret = ihid_write(iplc, pkt, 2);
	ihid_cts(iplc, 0);

	/* Got a clear to send, but did the bytes get echoed back? */
	while (to) {
		if (rb_avail(iplc, rs, len) < 2) {
			ihid_cts(iplc, 1);
			usleep(60);
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
 * ihid_write
 *
 * Send bytes to the interface.  This function does not wait for the PLC
 * to echo back the data sent.
 *
 * Parameters:
 *   iplc     interface handle
 *   pkt      buffer with data to send
 *   len      number of bytes to send
 */
int ihid_write(iusb_t *iplc, unsigned char *pkt, int len)
{
	int *fd;
	int cnt;
	struct hiddev_usage_ref out;
	struct hiddev_report_info report;

	fd = (int *)iplc->vfd;

	/* Fill up the USB output buffer */
	for (cnt = 0; cnt < len + 1; cnt++) {
		out.report_type = HID_REPORT_TYPE_OUTPUT;
		out.report_id = HID_REPORT_ID_FIRST;
		out.field_index = 0;
		out.usage_index = cnt;
		out.usage_code = 0;
		out.value = pkt[cnt];
		ioctl(*fd, HIDIOCSUSAGE, &out);
	}

	/* Send the buffer to the device */
	report.report_type = HID_REPORT_TYPE_OUTPUT;
	report.report_id = HID_REPORT_ID_FIRST;
	report.num_fields = 0;
	ioctl(*fd, HIDIOCSREPORT, &report);

	return len;
}


static int ihid_read_usb (int fd, unsigned char *pkt, int *count)
{
	struct hiddev_event event;
	int cts = 0;
	int len = 0;

	*count = 0;
	while (read(fd, &event, sizeof(struct hiddev_event)) > 0) {
		event.hid &= 0xff;
		/* Check for CTS */
		if ((event.hid == 1) && (event.value > 8)) {
			cts = 1;
			event.value &= 0x0f;
		}

		if (event.hid == 1) { /* data is number of bytes in packet */
			*count = event.value;
		} else if (event.hid == 8) { /* last byte in packet */
			*pkt = event.value;
			len++;
			return cts;
		} else {
			*pkt = event.value;
			pkt++;
			len++;
		}

		if (len > 8) {
			printf("In ihid_read_usb: packet length exceeded!\n");
			return cts;
		}
	}

	return cts;
}
