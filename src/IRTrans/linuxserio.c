/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifdef LINUX

#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/file.h>

#include "remote.h"
#include "errcode.h"
#include "lowlevel.h"
#include "serio.h"
#include "global.h"


#define BAUDRATE	B38400

char SerialDevice[256];


void msSleep (long time)
{
	struct timeval tv;

	tv.tv_sec = time / 1000;
	tv.tv_usec = (time % 1000) * 1000;

	select (0,NULL,NULL,NULL,&tv);
}



int WriteSerialStringEx (DEVICEINFO *dev,byte pnt[],int len)
{
	int res,stat;

	res = write (dev->io.comport,pnt,len);
	if (res != len) {
		if (dev->io.if_type == IF_USB && !(mode_flag & NO_RECONNECT)) {
			close (dev->io.comport);
			while (stat) {
				stat = OpenSerialPortEx (dev->io.node,&dev->io.comport);
				if (stat) sleep (10);
			}
		}
		else {
			log_print ("IRTrans Connection lost. Aborting ...\n",LOG_FATAL);
			exit (-1);
		}
		return (ERR_TIMEOUT);
	}
	return (0);
}


int ReadSerialStringEx (DEVICEINFO *dev,byte pnt[],int len,word timeout)
{
	int bytes = 0;
	int total = 0;
	struct timeval tv;
	fd_set fs;

	while (total < len) {
		FD_ZERO (&fs);
		FD_SET (dev->io.comport,&fs);
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		bytes = select (dev->io.comport+1,&fs,NULL,NULL,&tv);
		
		// 0 bytes or error
		if( bytes <= 0 )
		{
			return total;
		}
		
		bytes = read (dev->io.comport,pnt+total,len-total);
		total += bytes;
	}
	
	return total;
}

void FlushCom ()
{
}


int ReadSerialString (byte pnt[],int len,word timeout)
{
	return (0);
}


void WriteSerialString (byte pnt[],int len)
{
}

void FlushComEx(HANDLE fp)
{
	int bytes;
	struct timeval tv;
	fd_set fs;
	char dummy[256];

	FD_ZERO (&fs);
	FD_SET (fp,&fs);
	tv.tv_sec = 0;
	tv.tv_usec = 10000;
	bytes = select (fp+1,&fs,NULL,NULL,&tv);
	if (!bytes) return;
	bytes = read (fp,dummy,256);

}




int OpenSerialPortEx (char Pname[],int *port)
{
	struct termios portterm;
	if ((*port = open(Pname, O_RDWR | O_NOCTTY)) < 0) return (ERR_OPEN);
 
	if (!isatty(*port)) {
		close(*port);
		return (ERR_OPEN);
	}

#ifndef DBOX
	if (flock(*port, LOCK_EX | LOCK_NB) < 0) {
		close(*port);
		return (ERR_FLOCK);
	}
#endif

	portterm.c_cflag = CS8 | CREAD | CLOCAL;

	portterm.c_cc[VMIN] = 1; 
	portterm.c_cc[VTIME] = 0;

  	cfsetispeed(&portterm, BAUDRATE);
	cfsetospeed(&portterm, BAUDRATE);

	portterm.c_lflag = 0;

	portterm.c_iflag = IGNBRK;
	portterm.c_oflag = 0;


	tcflush(*port, TCIOFLUSH);
	if (tcsetattr(*port, TCSANOW, &portterm) < 0) {
		close(*port);
		return (ERR_STTY);
	}
	msSleep (1000);  
	
	tcflush(*port, TCIOFLUSH);

	return (0);
}


#ifdef DBOX

tcflush (int fd,int mode)
{
	char st[1024];
	do {
	} while (ReadSerialString (st,1000,10) == 1000);
}

#endif


#endif

