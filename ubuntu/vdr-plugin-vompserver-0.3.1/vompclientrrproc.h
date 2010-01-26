/*
    Copyright 2008 Chris Tallon

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

#ifndef VOMPCLIENTRRPROC_H
#define VOMPCLIENTRRPROC_H

#include "thread.h"
#include "responsepacket.h"
#include <queue>
#include "serialize.h"

extern bool ResumeIDLock;

using namespace std;

class VompClient;
class Log;

class RequestPacket
{
  public:
    RequestPacket(ULONG requestID, ULONG opcode, UCHAR* data, ULONG dataLength)
     : requestID(requestID), opcode(opcode), data(data), dataLength(dataLength) {}
    
    ULONG requestID;
    ULONG opcode;
    UCHAR* data;
    ULONG dataLength;
};

typedef queue<RequestPacket*> RequestPacketQueue;

class VompClientRRProc : public Thread
{
  public:
    VompClientRRProc(VompClient& x);
    ~VompClientRRProc();
    
    bool init();
    bool recvRequest(RequestPacket*);

  private:
    bool processPacket();
    void sendPacket(SerializeBuffer *b);
  
#ifndef VOMPSTANDALONE
    int processGetRecordingsList();
    int processDeleteRecording();
    int processMoveRecording();
    int processGetChannelsList();
    int processStartStreamingChannel();
    int processGetBlock();
    int processStopStreaming();
    int processStartStreamingRecording();
    int processGetChannelSchedule();
    int processGetTimers();
    int processSetTimer();
    int processPositionFromFrameNumber();
    int processFrameNumberFromPosition();
    int processGetIFrame();
    int processGetRecInfo();
    int processGetMarks();
    int processGetChannelPids();
    int processDeleteTimer();
    int processReScanRecording();           // FIXME obselete
#endif
    int processLogin();
    int processConfigSave();
    int processConfigLoad();
    int processGetMediaList();
    int processOpenMedia();
    int processGetMediaBlock();
    int processGetMediaInfo();
    int processCloseMediaChannel();
    int processGetLanguageList();
    int processGetLanguageContent();

    void threadMethod();

    VompClient& x;
    RequestPacket* req;
    RequestPacketQueue req_queue;
    ResponsePacket* resp;
    
    Log* log;
};

#endif

