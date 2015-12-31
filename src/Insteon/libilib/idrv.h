/*
 * $RCSfile: idrv.h,v $  $Revision: 1.3 $  $Name:  $
 * $Id: idrv.h,v 1.3 2007/04/21 03:23:27 bpaauwe Exp $
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
 * ----------------------------------------------------------------------------
 */
#ifndef _IDRV_H
#define _IDRV_H

/* #include <hid.h> */

#define VERSION "libilib version 0.2.17.a (c) 2006, 2007 Bob Paauwe"
#define IPLC_VENDOR     0x10bf
#define IPLC_PRODUCT    0x0004
#define RING_SIZE       10000

typedef void ilib_t;

/*
 * This structure holds all the information needed to communicate with
 * the PLC.  The vfd will be one of three types, depending on which
 * communication method is initialized.
 *    struct usb_dev_handle *    - when using libusb
 *    HIDInterface *             - when using libhid
 *    int *                      - when using kernel driver
 *    int *                      - when using serial driver
 *
 * During initialization, the function table will be filled in with
 * functions the implement the chosen communication method.  This allows
 * new communications methods to be defined simply by implimenting these
 * functions.
 */
typedef struct _iusb_t {
	void *vfd;  /* interface descriptor */
	unsigned char plc_id[3];

	/* Function dispatch table */
	void *(*init)(char *dev);
	void (*close)(struct _iusb_t *iplc);
	int (*read_byte)(struct _iusb_t *iplc);
	int (*cts)(struct _iusb_t *iplc, int cts);
	int (*send_pkt)(struct _iusb_t *iplc, unsigned char *pkt, int len);
	int (*send)(struct _iusb_t *iplc, unsigned char *pkt, int len);

	/* Ring buffer */
	unsigned char *ring_buffer;
	int ring_head;
	int ring_tail;
} iusb_t;

#define USE_IPLC   0
#define USE_LIBUSB 1
#define USE_LIBHID 2
#define USE_SERIAL 3
#define USE_HIDDEV 4

/* These fuctions are exported to the application. */
iusb_t *iusb_open(int method, char *dev);
void iusb_close(iusb_t *iplc);

/*
 * These functions are used by the mid level functions. They should not
 * be called directly from the application.
 */
int send_cmd(iusb_t *iplc, unsigned char cmd, unsigned char *data, int len);
int plc_read_response(iusb_t *iplc, unsigned char *buf, int size,
		unsigned char matach);
int chksum(unsigned char *data, int size);

/*
 * These functions are available to the interface dependent code.
 */
int rb_avail(iusb_t *iplc, int index, int len);
int echo_match (iusb_t *iplc, int st, unsigned char *pkt, int len);
void print_ring(iusb_t *iplc);

#endif
