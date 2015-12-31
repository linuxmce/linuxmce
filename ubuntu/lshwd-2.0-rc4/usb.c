/*
 * USB enum functions, by z4ziggy at yahoo dot com.
 * ripped and stripped from lsusb which is originally by :
 *   Thomas Sailer (sailer@ife.ee.ethz.ch) 
 *
 */

//#define DEBUG(s...) fprintf(stderr, s)
#define DEBUG(s...) ;

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

#include <linux/types.h>
#include <linux/usbdevice_fs.h>
//#include "usbdevice_fs.h"
#include <linux/usb.h>

#include "usb.h"

#define CTRL_RETRIES	 50
#define CTRL_TIMEOUT	100	/* milliseconds */

/* ---------------------------------------------------------------------- */

static int usb_control_msg(int fd, u_int8_t requesttype, u_int8_t request, u_int16_t value,
			   u_int16_t index, unsigned int size, void *data)
{
	struct usbdevfs_ctrltransfer ctrl;
	int result, try;

	ctrl.bRequestType = requesttype;
	ctrl.bRequest = request;
	ctrl.wValue = value;
	ctrl.wIndex = index;
	ctrl.wLength = size;
	ctrl.timeout = 1000;
	ctrl.data = data;
 	ctrl.timeout = CTRL_TIMEOUT; 
 	try = 0;
 	do {
 	  	result = ioctl(fd, USBDEVFS_CONTROL, &ctrl);
 		try++;
 	} while (try < CTRL_RETRIES && result == -1 && errno == ETIMEDOUT);
 	return result;
}

/* ---------------------------------------------------------------------- */

int usb_get_string(int fd, char *buf, size_t size, u_int8_t id, u_int16_t lang)
{
	unsigned char b[256];
	unsigned int i;
	int ret;

	if (size < 1)
		return 0;
	//*buf = 0;
	/* string ID 0 means no string */
	if (!id || fd == -1)
		return 0;

	b[0] = b[1] = 0xbf;
	ret = usb_control_msg(fd, USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) | id, 0, sizeof(b), b);
	if (ret < 0) {
		DEBUG( "cannot get string descriptor %d, error = %s(%d)\n", id, strerror(errno), errno);
		return 0;
	}
	if (ret < 2 || b[0] < 2 || b[1] != USB_DT_STRING) {
		DEBUG( "string descriptor %d invalid (%02x %02x; len=%d)\n", id, b[0], b[1], ret);
		return 0;
	}
	for (i = 0; i < ((b[0] - 2) / 2); i++)
		buf[i] = b[2+2*i];
	buf[i] = 0;
        return i;
}

/* ---------------------------------------------------------------------- */

int usb_get_procfs_driver (int fd, int interface, char *module)
{
	struct usbdevfs_getdriver driver;
	int ret;
	
	driver.interface = interface;
	*driver.driver = 0;
	ret =  ioctl (fd, USBDEVFS_GETDRIVER, &driver);
	if (*driver.driver) strcpy(module, driver.driver);
	return ret;
}
