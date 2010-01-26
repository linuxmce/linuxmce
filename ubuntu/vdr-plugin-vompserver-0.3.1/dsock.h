/*
    Copyright 2004-2005 Chris Tallon

    This file is part of VOMP.

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef DSOCK_H
#define DSOCK_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "defines.h"
#include "log.h"

#define MAXBUFLEN 2000
const char DSOCKDEBUG = 0;
typedef unsigned char uchar;

class DatagramSocket
{
  public:
    DatagramSocket();
    ~DatagramSocket();
    bool init(USHORT);                 // port
    void shutdown();
    unsigned char waitforMessage(unsigned char); // int =0-block =1-new wait =2-continue wait
    void send(const char *, USHORT, char *, int); // send wants: IP Address ddn style, port,
                                            // data, length of data

    char*  getData(void)             { return buf; }      // returns a pointer to the data
    char*  getFromIPA(void)          { return fromIPA; }  // returns a pointer to from IP address
    USHORT getFromPort(void) const   { return fromPort; } // returns the sender port number
    int    getDataLength(void) const { return mlength; }  // returns data length

    ULONG  getMyIP(ULONG targetIP);
    static struct in_addr myIPforIP(struct in_addr targetIP);

  private:
    Log* log;
    int initted;
    int socketnum;                  // Socket descriptor
    USHORT myPort;                  // My port number
    struct sockaddr_in myAddr;      // My address
    struct sockaddr_in theirAddr;   // User address
    socklen_t addrlen;              // length of sockaddr struct
    char buf[MAXBUFLEN];            // main data buffer
    char fromIPA[20];               // from string (ip address)
    USHORT fromPort;                // which port user sent on
    int mlength;                    // length of message
    struct timeval tv;
    fd_set readfds;
};

#endif
