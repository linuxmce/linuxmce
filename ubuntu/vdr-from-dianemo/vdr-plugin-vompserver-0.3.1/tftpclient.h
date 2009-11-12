/*
    Copyright 2006 Chris Tallon

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

#ifndef TFTPCLIENT_H
#define TFTPCLIENT_H

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#ifndef VOMPSTANDALONE
#include <vdr/plugin.h>
#endif

#include "defines.h"
#include "log.h"
#include "dsock.h"
#include "thread.h"

class TftpClient : public Thread
{
  public:
    TftpClient();
    virtual ~TftpClient();

    int run(char* baseDir, char* ip, USHORT port, UCHAR* data, int length);
    int shutdown();

  private:
    Log* log;
    DatagramSocket ds;
    char* baseDir;
    char peerIP[17];
    USHORT peerPort;
    UCHAR buffer[600];
    int bufferLength;
    time_t lastCom;
    FILE* file;
    int state;
    // 0 = start
    // 1 = awaiting ack
    // 2 = transfer finished, awaiting last ack
    USHORT blockNumber;

    void threadMethod();
    void threadPostStopCleanup();

    int processMessage(UCHAR* data, int length);
    int processReadRequest(UCHAR* data, int length);
    int processAck(UCHAR* data, int length);

    int openFile(char* filename);
    int sendBlock();
    void transmitBuffer();
};

#endif
