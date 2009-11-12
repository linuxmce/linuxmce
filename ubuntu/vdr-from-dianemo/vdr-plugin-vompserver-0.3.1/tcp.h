/*
    Copyright 2004-2005 Chris Tallon
    Copyright 2003-2004 University Of Bradford

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

#ifndef TCP_H
#define TCP_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// For TCP_KEEPIDLE and co
#include <netinet/tcp.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>

#include "log.h"


typedef unsigned char UCHAR;
typedef unsigned short USHORT;

class TCP
{
  public:
    // Creation
    TCP(int tsocket);
    ~TCP();
    void assignSocket(int tsocket);

    // Setup
    void disableReadTimeout();
    void setNonBlocking();
    int setSoKeepTime(int timeOut);

    int connectTo(char *host, unsigned short port);
    int sendPacket(UCHAR*, size_t size);
//    UCHAR* receivePacket();
    int readData(UCHAR* buffer, int totalBytes);
    
    // Get methods
    int isConnected();
    int getDataLength();

    static void dump(unsigned char* data, USHORT size);
    static UCHAR dcc(UCHAR c);

  private:
    Log* log;
    int sock;
    int connected;
    int readTimeoutEnabled;
    int dataLength;
    pthread_mutex_t sendLock;
    
    void cleanup();
};

#endif
