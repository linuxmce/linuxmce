/*
    Copyright 2004-2008 Chris Tallon

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

/*
  The new async protocol design forced the server side change from the mvpclient
  class to the mess it is now. Maybe in a couple of versions time it will become
  more apparent how better to design all this.
  
  The reason for the class split is that with the current Thread class system,
  you can only have one thread per object. The vompclient class would have needed
  two, one for RR stuff and one for the keepalives.
  
  So the new design is this: the VompClient class represents one connection from
  one client as the MVPClient class did before. But because two threads are now
  needed, a helper class VompClientRRProc contains all the RR processing functions,
  and the thread needed to process them. All the state data is still kept in the
  VompClient class.
*/

#ifndef VOMPCLIENT_H
#define VOMPCLIENT_H

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <endian.h>

#include <unistd.h> // sleep

#ifndef VOMPSTANDALONE
class RecPlayer;
class MVPReceiver;
class cChannel;
class cRecordings;
#endif

#include "defines.h"
#include "tcp.h"
#include "config.h"
#include "media.h"
#include "i18n.h"
#include "vompclientrrproc.h"

class ResponsePacket;
class ServerMediaFile;
class SerializeBuffer;
class MediaPlayer;

class VompClient
{
  friend class VompClientRRProc;

  public:
    VompClient(Config* baseConfig, char* configDir, int tsocket);
    ~VompClient();

    int run();
    // not for external use
    void run2();
    static int getNrClients();

  private:
    static int nr_clients;
    void incClients();
    void decClients();

    static ULLONG ntohll(ULLONG a);
    static ULLONG htonll(ULLONG a);
    
    VompClientRRProc rrproc;
    pthread_t runThread;
    int initted;
    Log* log;
    TCP tcp;
    Config config;
    Config* baseConfig;
    I18n i18n;
    bool loggedIn;
    char* configDir;

    //void cleanConfig();

#ifndef VOMPSTANDALONE
    cChannel* channelFromNumber(ULONG channelNumber);
    void writeResumeData();

    MVPReceiver* lp;
    cRecordings* recordingManager;
    RecPlayer* recplayer;
#endif
    MediaPlayer *media;
    ServerMediaFile *mediaprovider;
};

#endif

