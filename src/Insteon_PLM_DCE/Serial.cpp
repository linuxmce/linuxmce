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


#include <iostream>

#include "Serial.h"

#define BAUDRATE   B19200

int OpenSerialPortEx (const char Pname[],int *port)
{
        struct termios portterm;
        if ((*port = open(Pname, O_RDWR | O_NOCTTY)) < 0) return (ERR_OPEN);
 	
 
        if (!isatty(*port)) {
                close(*port);
                return (ERR_OPEN);
        }
        if (flock(*port, LOCK_EX | LOCK_NB) < 0) {
                close(*port);
                return (ERR_FLOCK);
        }
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
        usleep (1000);  
       tcflush(*port, TCIOFLUSH);

        return (0);
}


int ReadSerialStringEx (int dev,char pnt[],int len,long timeout)
{
        int bytes = 0;
        int total = 0;
        struct timeval tv;
        fd_set fs;

        while (total < len) {
                FD_ZERO (&fs);
                FD_SET (dev,&fs);
                tv.tv_sec = 0;
                tv.tv_usec = 100000;
                bytes = select (FD_SETSIZE,&fs,NULL,NULL,&tv);

                // 0 bytes or error
                if( bytes <= 0 )
                {
                        return total;
                }

                bytes = read (dev,pnt+total,len-total);
                total += bytes;
        }

        return total;
}

int WriteSerialStringEx (int dev,char pnt[],int len)
{
        int res;

        res = write (dev,pnt,len);
        if (res != len) {
                        exit (-1);
                return (ERR_TIMEOUT);
        }
        tcflush(dev, TCIOFLUSH);

        return (0);
}



