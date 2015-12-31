/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*********************************************************************
 *                
 * Filename:        obex_cable.c
 * Original author: Pontus Fuchs <pontus.fuchs@tactel.se>
 *
 *     Copyright (c) 1999, 2000 Pontus Fuchs, All Rights Reserved.
 *      
 *     This library is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU Lesser General Public
 *     License as published by the Free Software Foundation; either
 *     version 2 of the License, or (at your option) any later version.
 *
 *     This library is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 *
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this library; if not, write to the Free Software
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *     MA  02111-1307  USA
 *     
 ********************************************************************/

/*
   $Id: obex_socket.c,v 1.6 2002/05/15 13:08:06 kds Exp $

   OBEX serial port transport functions.

   Adapted for Affix, Fixes:       Imre Deak <ext-imre.deak@nokia.com>
   Adapted for Bluez, Fixes marked [REV]:  reverend@unrooted.net
*/
	 
#include <stdio.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include <glib.h>

#include <openobex/obex.h>

/*
 * [REV]
 *
 * I've moved the necessary parts of these two includes directly into here.
 * The BTDEBUG and BTERROR macros and the cobex_context struct.
 *
 * #include <affix/btcore.h>
 * #include "obex_server.h"
 */


#define BTDEBUG(fmt,args...) printf(fmt, ##args);
#define BTERROR(fmt,args...) printf(fmt, ##args);

struct cobex_context {
    const char *portname;
    int rfd, wfd;
    char inputbuf[4096]; 
    struct termios oldtio, newtio;
};

static void cobex_cleanup(struct cobex_context *gt, gboolean force);

//
// Open serial port.
//
static gint cobex_init(struct cobex_context *gt)
{
	BTDEBUG("");

	if( gt->rfd == -1 ) {
		if( gt->portname != NULL ) {
			// open device
			if( (gt->rfd = open(gt->portname, O_RDWR | O_NONBLOCK | O_NOCTTY, 0)) < 0 ) {
				perror("Can' t open tty");
				return -1;
			}
			gt->wfd = gt->rfd;

			tcgetattr(gt->rfd, &gt->oldtio);
			bzero(&gt->newtio, sizeof(struct termios));
			gt->newtio.c_cflag = B115200 | CS8 | CREAD; // T.Koba for RFCOMM
			gt->newtio.c_iflag = IGNPAR;
			gt->newtio.c_oflag = 0;
			tcflush(gt->rfd, TCIFLUSH);
			tcsetattr(gt->rfd, TCSANOW, &gt->newtio);
		} else {
			gt->rfd = 0;
			gt->wfd = 1;
		}
	}

	return 1;
}

//
// Close down. If force is TRUE. Try to break out of OBEX-mode.
//
static void cobex_cleanup(struct cobex_context *gt, gboolean force)
{
	BTDEBUG("");
	if( gt->portname != NULL ) {
		if(force) {
			// Send a break to get out of OBEX-mode
			if (ioctl(gt->rfd, TCSBRKP, 0) < 0) {
				BTERROR("Unable to send break!");
			}
		}
		close(gt->rfd);
	} 
	
	gt->rfd = gt->wfd = -1;
}

//
// Open up cable OBEX
//
struct cobex_context *cobex_open(const gchar *port)
{
	struct cobex_context *gt;
	
	BTDEBUG("");
	gt = g_new0(struct cobex_context, 1);
	if (gt == NULL)
		return NULL;
	
	gt->rfd = gt->wfd = -1;
	gt->portname = port;
	return gt;
}

struct cobex_context *cobex_setsocket(const int fd)
{
	struct cobex_context *gt;
	
	BTDEBUG("");
	gt = g_new0(struct cobex_context, 1);
	if (gt == NULL)
		return NULL;
	
	gt->rfd = gt->wfd = fd;
	gt->portname = NULL;
	return gt;
}

int cobex_getsocket(struct cobex_context *gt)
{
	return gt->rfd;
}

	
//
// Close down cable OBEX.
//
void cobex_close(struct cobex_context *gt)
{
	g_free(gt);
}
	
//
// Do transport connect or listen
//
gint cobex_connect(obex_t *handle, gpointer userdata)
{
	struct cobex_context *gt;

	BTDEBUG("");
	
	gt = userdata;

	if(gt->rfd >= 0) {
		BTDEBUG("fd already exist. Using it");
		return 1;

	}
	if(cobex_init(gt) < 0)
		return -1;
	return 1;
}

//
// Do transport disconnect
//
gint cobex_disconnect(obex_t *handle, gpointer userdata)
{
	struct cobex_context *gt;

	BTDEBUG("");
	gt = userdata;

	cobex_cleanup(gt, FALSE);
	return 1;
}

//
//  Called when data needs to be written
//
gint cobex_write(obex_t *handle, gpointer userdata, guint8 *buffer, gint length)
{
	struct cobex_context *gt;
	gint actual = 0;
	gint pos = 0;

	gt = userdata;

    /*
     * [REV]
     * For some reason the init function wasn't getting called before this call
     * to write.  Probably my own fault, something I ripped out must have in 
     * the initial cobex_connect() callback from OpenOBEX.  Fortunately for me
     * it works fine if I just call the init from the first write.  The init
     * sets the wfd field so that this call gets skipped on all writes after 
     * the first.
     */
	if(gt->wfd < 0) {
	  if(cobex_init(gt) < 0) {
          return( -1 );
      }
    }

	while (actual < length)	{
		int frag;
		frag = write(gt->wfd, &buffer[pos], length - pos);
		if (frag < 0) {
			if (errno == EAGAIN) {
				frag = 0;
				BTDEBUG("Temp. no resource avail.");
			} else {
				BTERROR("Write error:%s", strerror(errno));
				return frag;
			}
		}
		actual += frag;
		pos += frag;
		BTDEBUG("Wrote %d fragment", frag);
	} 
	BTDEBUG("Wrote %d bytes (expected %d)", actual, length);
	return actual;
}

//
// Called when more data is needed.
//
gint cobex_handle_input(obex_t *handle, gpointer userdata, gint timeout)
{
	gint actual;
	struct cobex_context *gt;
	struct timeval time;
	fd_set fdset;
        gint ret;
	gint16 expectlen;
	
	BTDEBUG("");

	gt = userdata;
	
	/* Return if no fd */
	if(gt->rfd < 0)
		return -1;

	time.tv_sec = timeout;
	time.tv_usec = 0;

	FD_ZERO(&fdset);
	FD_SET(gt->rfd, &fdset);

	// for fragment read  T.Koba
	actual = 0;
	expectlen = 0;
	do {
		gint len;
		fd_set fdset1 = fdset;

		ret = select(gt->rfd+1, &fdset1, NULL, NULL, &time);
	
		/* Check if this is a timeout (0) or error (-1) */
		if (ret < 0) {
			BTERROR("Error (%d)", ret);
			return -1;
		}
		if (ret == 0)
			len = 0;
		else {
			len = read(gt->rfd, &gt->inputbuf[actual], sizeof(gt->inputbuf)-actual);
			if( len < 0 ){
				if (errno == -EAGAIN) {
					BTDEBUG("Temp. no resource available.");
					len = 0;
				} else {
					BTDEBUG("read error (%d)", len);
					return len;
				}
			} else if (len == 0) {
				BTDEBUG("Connection terminated.");
				return -1;
			}
		}
		actual += len;
		if( (expectlen == 0) && (actual >= 3) ) {
			
			expectlen = g_ntohs(*(guint16*)(gt->inputbuf+1));
			BTDEBUG("expect %d bytes", expectlen);
			if( expectlen > sizeof(gt->inputbuf) )
			{
				BTDEBUG("cobex_context buffer size is too small!!!");
				exit(1);
			}
		}
	} while( (actual < expectlen) || (expectlen == 0) );

	BTDEBUG("Read %d bytes", actual);
	OBEX_CustomDataFeed(handle, gt->inputbuf, actual);
	return actual;
}

