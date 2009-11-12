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

#include "mediaplayer.h"
#include "media.h"
#include "servermediafile.h"
#include "vdrcommand.h"
#include "vompclient.h"

#include "responsepacket.h"

#ifndef VOMPSTANDALONE
#include <vdr/channels.h>
#include <vdr/recording.h>
#include "recplayer.h"
#include "mvpreceiver.h"
#endif



pthread_mutex_t threadClientMutex;
int VompClient::nr_clients = 0;


VompClient::VompClient(Config* cfgBase, char* tconfigDir, int tsocket)
 : rrproc(*this), tcp(tsocket), i18n(tconfigDir)
{
#ifndef VOMPSTANDALONE
  lp = NULL;
  recplayer = NULL;
  recordingManager = NULL;
#endif
  log = Log::getInstance();
  loggedIn = false;
  configDir = tconfigDir;
  log->log("Client", Log::DEBUG, "Config dir: %s", configDir);
  baseConfig = cfgBase;
  incClients();
  media=new MediaPlayer();
  mediaprovider=new ServerMediaFile(cfgBase,media);
  
  rrproc.init();
}

VompClient::~VompClient()
{
  log->log("Client", Log::DEBUG, "Vomp client destructor");
#ifndef VOMPSTANDALONE  
  if (lp)
  {
    delete lp;
    lp = NULL;
  }
  else if (recplayer)
  {
    writeResumeData();

    delete recplayer;
    delete recordingManager;
    recplayer = NULL;
    recordingManager = NULL;
  }
#endif
  //if (loggedIn) cleanConfig();
  decClients();
  
  delete media;
  delete mediaprovider;
}

void VompClient::incClients()
{
  pthread_mutex_lock(&threadClientMutex);
  VompClient::nr_clients++;
  pthread_mutex_unlock(&threadClientMutex);
}

void VompClient::decClients()
{
  pthread_mutex_lock(&threadClientMutex);
  VompClient::nr_clients--;
  pthread_mutex_unlock(&threadClientMutex);
}

int VompClient::getNrClients()
{
  int nrClients;
  pthread_mutex_lock(&threadClientMutex);
  nrClients = VompClient::nr_clients;
  pthread_mutex_unlock(&threadClientMutex);
  return nrClients;
}

/*
void VompClient::cleanConfig()
{
  log->log("Client", Log::DEBUG, "Clean config");

#ifndef VOMPSTANDALONE

  cRecordings Recordings;
  Recordings.Load();

  int numReturns;
  int length;
  char* resumes = config.getSectionKeyNames("ResumeData", numReturns, length);
  char* position = resumes;
  for(int k = 0; k < numReturns; k++)
  {
    log->log("Client", Log::DEBUG, "EXAMINING: %i %i %p %s", k, numReturns, position, position);

    cRecording* recording = Recordings.GetByName(position);
    if (!recording)
    {
      // doesn't exist anymore
      log->log("Client", Log::DEBUG, "Found a recording that doesn't exist anymore");
      config.deleteValue("ResumeData", position);
    }
    else
    {
      log->log("Client", Log::DEBUG, "This recording still exists");
    }

    position += strlen(position) + 1;
  }

  delete[] resumes;
#endif
} */

void VompClientStartThread(void* arg)
{
  VompClient* m = (VompClient*)arg;
  m->run2();
  // Nothing external to this class has a reference to it
  // This is the end of the thread.. so delete m
  delete m;
  pthread_exit(NULL);
}

int VompClient::run()
{
  if (pthread_create(&runThread, NULL, (void*(*)(void*))VompClientStartThread, (void *)this) == -1) return 0;
  log->log("Client", Log::DEBUG, "VompClient run success");
  return 1;
}

void VompClient::run2()
{
  // Thread stuff
  sigset_t sigset;
  sigfillset(&sigset);
  pthread_sigmask(SIG_BLOCK, &sigset, NULL);
  pthread_detach(runThread);  // Detach

//  tcp.disableReadTimeout();

//  tcp.setSoKeepTime(3);
  tcp.setNonBlocking();

  ULONG channelID;
  ULONG requestID;
  ULONG opcode;
  ULONG extraDataLength;
  UCHAR* data;
  
  ULONG kaTimeStamp;

  while(1)
  {
    log->log("Client", Log::DEBUG, "Waiting");
    
    if (!tcp.readData((UCHAR*)&channelID, sizeof(ULONG)))
    {
      // If this read fails then the client just hasn't sent anything.
      // If any of the lower reads fail, then break, the connection is probably dead

      // check connection is ok
//      if (tcp.isConnected()) continue;
      
      log->log("Client", Log::DEBUG, "Disconnection detected");
      break;
    }     
    
    channelID = ntohl(channelID);
    if (channelID == 1)
    {
      if (!tcp.readData((UCHAR*)&requestID, sizeof(ULONG))) break;
      requestID = ntohl(requestID);

      if (!tcp.readData((UCHAR*)&opcode, sizeof(ULONG))) break;
      opcode = ntohl(opcode);

      if (!tcp.readData((UCHAR*)&extraDataLength, sizeof(ULONG))) break;
      extraDataLength = ntohl(extraDataLength);
      if (extraDataLength > 200000) // a random sanity limit
      {
        log->log("Client", Log::ERR, "ExtraDataLength > 200000!");
        break;
      }

      if (extraDataLength)
      {
        data = (UCHAR*)malloc(extraDataLength);
        if (!data)
        {
          log->log("Client", Log::ERR, "Extra data buffer malloc error");
          break;
        }
        
        if (!tcp.readData(data, extraDataLength))
        {
          log->log("Client", Log::ERR, "Could not read extradata");
          free(data);
          break;
        }      
      }
      else
      {
        data = NULL;
      }

      log->log("Client", Log::DEBUG, "Received chan=%lu, ser=%lu, op=%lu, edl=%lu", channelID, requestID, opcode, extraDataLength);

      if (!loggedIn && (opcode != 1))
      {
        log->log("Client", Log::ERR, "Not logged in and opcode != 1");
        if (data) free(data);
        break;
      }

      RequestPacket* req = new RequestPacket(requestID, opcode, data, extraDataLength);
      rrproc.recvRequest(req);
    }
    else if (channelID == 3)
    {
      if (!tcp.readData((UCHAR*)&kaTimeStamp, sizeof(ULONG))) break;
      kaTimeStamp = ntohl(kaTimeStamp);

      log->log("Client", Log::DEBUG, "Received chan=%lu kats=%lu", channelID, kaTimeStamp);    

      UCHAR buffer[8];
      *(ULONG*)&buffer[0] = htonl(3); // KA CHANNEL
      *(ULONG*)&buffer[4] = htonl(kaTimeStamp);
      if (!tcp.sendPacket(buffer, 8))
      {
        log->log("Client", Log::ERR, "Could not send back KA reply");
        break;
      }      
    }
    else
    {
      log->log("Client", Log::ERR, "Incoming channel number unknown");
      break;
    }
  }
}

ULLONG VompClient::ntohll(ULLONG a)
{
  return htonll(a);
}

ULLONG VompClient::htonll(ULLONG a)
{
  #if BYTE_ORDER == BIG_ENDIAN
    return a;
  #else
    ULLONG b = 0;

    b = ((a << 56) & 0xFF00000000000000ULL)
      | ((a << 40) & 0x00FF000000000000ULL)
      | ((a << 24) & 0x0000FF0000000000ULL)
      | ((a <<  8) & 0x000000FF00000000ULL)
      | ((a >>  8) & 0x00000000FF000000ULL)
      | ((a >> 24) & 0x0000000000FF0000ULL)
      | ((a >> 40) & 0x000000000000FF00ULL)
      | ((a >> 56) & 0x00000000000000FFULL) ;

    return b;
  #endif
}

#ifndef VOMPSTANDALONE

cChannel* VompClient::channelFromNumber(ULONG channelNumber)
{
  cChannel* channel = NULL;

  for (channel = Channels.First(); channel; channel = Channels.Next(channel))
  {
    if (!channel->GroupSep())
    {
      log->log("Client", Log::DEBUG, "Looking for channel %lu::: number: %i name: '%s'", channelNumber, channel->Number(), channel->Name());

      if (channel->Number() == (int)channelNumber)
      {
        int vpid = channel->Vpid();
#if VDRVERSNUM < 10300
        int apid1 = channel->Apid1();
#else
        int apid1 = channel->Apid(0);
#endif
        log->log("Client", Log::DEBUG, "Found channel number %lu, vpid = %i, apid1 = %i", channelNumber, vpid, apid1);
        return channel;
      }
    }
  }

  if (!channel)
  {
    log->log("Client", Log::DEBUG, "Channel not found");
  }

  return channel;
}

void VompClient::writeResumeData()
{
  /*config.setValueLong("ResumeData",
                          (char*)recplayer->getCurrentRecording()->FileName(),
                          recplayer->frameNumberFromPosition(recplayer->getLastPosition()) );*/

  /* write to vdr resume file */
  int resume = recplayer->frameNumberFromPosition(recplayer->getLastPosition());
  char* ResumeIdC = config.getValueString("General", "ResumeId");
  int ResumeId;
  if (ResumeIdC)
    ResumeId = atoi(ResumeIdC);
  else
    ResumeId = 0;

  while (ResumeIDLock)
	  cCondWait::SleepMs(100);
  ResumeIDLock = true;
  int OldSetupResumeID = Setup.ResumeID;
  Setup.ResumeID = ResumeId;				//UGLY: quickly change resumeid
#if VDRVERSNUM < 10703
  cResumeFile ResumeFile((char*)recplayer->getCurrentRecording()->FileName());	//get corresponding resume file
#else
  cResumeFile ResumeFile((char*)recplayer->getCurrentRecording()->FileName(),(char*)recplayer->getCurrentRecording()->IsPesRecording());	//get corresponding resume file
#endif
  Setup.ResumeID = OldSetupResumeID;			//and restore it back
  ResumeIDLock = false;
  ResumeFile.Save(resume);
  //isyslog("VOMPDEBUG: Saving resume = %i, ResumeId = %i",resume, ResumeId);
}

#endif

