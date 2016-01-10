/*******************************************************************************/
/* Copyright (C) 2004-2005  Chuck Cannon                                       */
/*                                                                             */
/* This program is free software; you can redistribute it and/or               */
/* modify it under the terms of the GNU General Public License                 */
/* as published by the Free Software Foundation; either version 2              */
/* of the License, or (at your option) any later version.                      */
/*                                                                             */
/* This program is distributed in the hope that it will be useful,             */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of              */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               */
/* GNU General Public License for more details.                                */
/*                                                                             */
/* You should have received a copy of the GNU General Public License           */
/* along with this program; if not, write to the Free Software                 */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */
/*******************************************************************************/

#ifdef HAISERIAL_SUPPORT

/* Includes */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include "hai_serial.h"

/* Defines */

#define HAI_SERIAL_TIMEOUT          3

/********************************************************************************/

/* Function to open serial connection */
HAIEXPORT int hai_serial_open(hai_comm_id *id, const char *dev, int baud)
{
    struct termios options;
    int baud_parm;

    /* Check arguments */
    if ((id == NULL) || (dev == NULL))
        return EHAIARGUMENT;

    /* Convert baud rate */
    switch (baud)
    {
        case 75 :
            baud_parm = B75;
            break;
        case 150 :
            baud_parm = B150;
            break;
        case 300 :
            baud_parm = B300;
            break;
        case 600 :
            baud_parm = B600;
            break;
        case 1200 :
            baud_parm = B1200;
            break;
        case 2400 :
            baud_parm = B2400;
            break;
        case 4800 :
            baud_parm = B4800;
            break;
        case 9600 :
            baud_parm = B9600;
            break;
        default :
            return EHAIARGUMENT;
    }

    /* Set connection type */
    id->serial_mode = 1;

    /* Open port */
    if ((id->s = open(dev, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
        return errno;

    /* Configure serial port */
    tcgetattr(id->s, &options);
    cfsetispeed(&options, baud_parm);
    cfsetospeed(&options, baud_parm);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CRTSCTS;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY | BRKINT | IMAXBEL | IUCLC | ICRNL
     | INLCR);
    options.c_iflag |= IGNPAR | IGNBRK;
    options.c_oflag &= ~OPOST;
    options.c_cc[VINTR]    = 0;     /* Ctrl-c */
    options.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    options.c_cc[VERASE]   = 0;     /* del */
    options.c_cc[VKILL]    = 0;     /* @ */
    options.c_cc[VEOF]     = 0;     /* Ctrl-d */
    options.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    options.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    options.c_cc[VSWTC]    = 0;     /* '\0' */
    options.c_cc[VSTART]   = 0;     /* Ctrl-q */
    options.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    options.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    options.c_cc[VEOL]     = 0;     /* '\0' */
    options.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    options.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    options.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    options.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    options.c_cc[VEOL2]    = 0;     /* '\0' */
    tcsetattr(id->s, TCSANOW, &options);

    return 0;
}

/********************************************************************************/

/* Function to close serial connection */
HAIEXPORT int hai_serial_close(const hai_comm_id *id)
{
	/* Check file handle */
	if (id->s == 0)
		return EHAISESSION;
	
    /* Close serial port */
    close (id->s);

    return 0;
}

/********************************************************************************/

/* Function to send serial message */
HAIEXPORT int hai_serial_send_msg(const hai_comm_id *id, const void *msg, int len)
{
    int cnt;

	/* Check file handle */
	if (id->s == 0)
		return EHAISESSION;

    /* Write data */
    if ((cnt = write(id->s, msg, len)) < 0)
        return errno;

    return 0;
}

/********************************************************************************/

/* Function to receive serial message */
HAIEXPORT int hai_serial_recv_msg(const hai_comm_id *id, void *msg, int *len)
{
    int retval, cnt, needed;
    struct timeval tv;
    fd_set rfds;

	/* Check file handle */
	if (id->s == 0)
		return EHAISESSION;

    /* Prepare read */
    needed = *len;
    *len = 0;

    /* Read until everything or timeout */
    while (needed > 0)
    {
        /* Wait for response */
        FD_ZERO(&rfds);
        FD_SET(id->s, &rfds);
        tv.tv_sec = HAI_SERIAL_TIMEOUT;
        tv.tv_usec = 0;
        retval = select(id->s + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1)
            return errno;
        else if (retval == 0)
            return EHAITIMEOUT;

        /* Read data */
        cnt = read(id->s, msg, needed);

        /* Update progress */
        needed -= cnt;
        msg += cnt;
        *len += cnt;
    }

    return 0;
}

#endif

