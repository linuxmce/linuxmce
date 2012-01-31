/**
 * Linux serial communication
 *
 * Copyright (C) Joseph Heenan, 1999 - 2008. All rights reserved.
 *
 * $Id: serial.c,v 1.7 2010-06-19 18:21:48 joseph Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#define _POSIX_SOURCE 1 /* POSIX compliant source */

#include "serial.h"

int serial_openport(const char *port, int baudrate, int stopbits)
{
    struct termios newtio;
    int fd;
    tcflag_t tc_baudrate;

    switch (baudrate)
    {
        case 115200:
            tc_baudrate = B115200;
            break;
        
        case 57600:
            tc_baudrate = B57600;
            break;

        case 19200:
            tc_baudrate = B19200;
            break;
        
        case 9600:
            tc_baudrate = B9600;
            break;

        case 300:
            tc_baudrate = B300;
            break;

        default:
            fprintf(stderr, "Unknown baudrate: %d\n", baudrate);
            exit(1);
    }

    /* O_NOCTTY: don't make it a controlling tty
     * O_NDELAY: don't sleep until DCD is set to the 'space' voltage */
    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
        perror(port);
        return -1;
    }
    /* make read not block even if no characters */
    if (fcntl(fd, F_SETFL, FNDELAY) != 0)
    {
        perror("fcntl");
        return -1;
    }

    memset( &newtio, 0, sizeof newtio );
    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    
    if (stopbits == 2)
    {
        newtio.c_cflag |= CSTOPB;
    }
    
    if (cfsetispeed(&newtio, tc_baudrate) != 0)
    {
        perror("cfsetispeed");
        return -1;
    }
    if (cfsetospeed(&newtio, tc_baudrate) != 0)
    {
        perror("cfsetospeed");
        return -1;
    }
    newtio.c_iflag = IGNPAR; /* ignore bytes with par. error, term dumb/raw */
    newtio.c_oflag = 0; /* raw output */

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

    if (tcsetattr(fd,TCSANOW,&newtio) != 0) /* with real baud rate */
    {
        perror("tcsetattr");
        return -1;
    }

    return fd;
}

void serial_terminal(int fd, unsigned char escape)
{
    struct termios oldstdtio, newstdtio;
    fd_set readfds;
    int maxfd, retval;
    unsigned char c;

    /* stop echo and buffering for stdin */
    tcgetattr(0, &oldstdtio);
    tcgetattr(0, &newstdtio); /* get working stdtio */
    newstdtio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newstdtio);

    /* loop and wait for input from serial or stdin */
    maxfd = fd + 1;
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds); /* stdin */
        FD_SET(fd, &readfds); /* serial port */

        /* wait for input, indefinitely */
        retval = select(maxfd, &readfds, NULL, NULL, NULL);

        if (retval == 0)
        {
            fprintf(stderr, "select() returned zero\n");
            break;
        }

        if ( retval == -1 )
        {
            perror("select");
            break;
        }

        if (FD_ISSET(0, &readfds))
        {
            /* keypress, send to serial port */
            retval = read(0, &c, 1);

            if (retval == 1)
            {
                printf("\nSend %d\n",c);
                if (c == escape)
                {
                    fprintf(stderr, "\nGot escape char, exitting\n");
                    break;
                }

                if (write(fd, &c, 1) != 1)
                {
                    perror("write(serialfd)");
                    break;
                }
            }
            else if (retval == 0)
            {
                fprintf(stderr, "Got EOF on stdin\n");
                break; /* EOF */
            }
            else
            {
                perror("read(stdin)");
            }
        }

        if (FD_ISSET(fd, &readfds))
        {
            /* serial input, send to stdout */
            retval = read(fd, &c, 1);

            if (retval == 1)
            {
                if (c == '\r')
                {
                    printf("[CR]");
                }
                else if (c == '\n')
                {
                    printf("[LF]\n");
                }
                else if (!isprint(c))
                {
                    printf("[%02X]", c);
                    fflush(stdout);
                }
                else if (write(1, &c, 1) != 1)
                {
                    perror("write(stdout)");
                    break;
                }
            }
            else if (retval == 0)
            {
                fprintf(stderr, "Got EOF on stdin\n");
                break; /* EOF */
            }
            else
            {
                perror("read(serial)");
            }
        }
    }

    /* return stdin to previous settings */
    tcsetattr(0,TCSANOW,&oldstdtio);
}

int serial_write(int fd, const char *ptr, int bytes)
{
    return write(fd, ptr, bytes);
}

int serial_read(int fd, unsigned char *ptr, int bytes)
{
    fd_set fds;
    int ret;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    ret = select(fd+1, &fds, NULL, NULL, NULL);
    if (ret != 1)
        return -1;
    
    return read(fd, ptr, bytes);
}

int serial_readtimeout(int fd, unsigned char *ptr, int bytes,
                       int msecs)
{
    fd_set fds;
    int ret;
    struct timeval tv;
    tv.tv_sec = msecs / 1000;
    tv.tv_usec = (msecs - tv.tv_sec * 1000) * 1000;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    ret = select(fd+1, &fds, NULL, NULL, &tv);
    //printf("select returned %d [errno=%d] [%d,%d]\n", ret, errno, (int)tv.tv_sec, (int)tv.tv_usec);
   
    if (ret == 0)
        return -2; /* timed out */
    if (ret != 1)
        return -1;
    
    ret = read(fd, ptr, bytes);
    //printf("read returned %d\n", ret);
    return ret;
}


void serial_close(int fd)
{
    (void) close(fd);
}
