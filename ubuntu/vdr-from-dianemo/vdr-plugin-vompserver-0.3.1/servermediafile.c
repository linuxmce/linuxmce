/*
    Copyright 2004-2005 Chris Tallon, Andreas Vogel

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "servermediafile.h"
#include "mediaproviderids.h"
#include "media.h"
#include "medialauncher.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include "log.h"


/* input buffer for reading dir */
#define BUFSIZE 2048
/* how long to wait until a list has to be finished (100ms units)*/
#define MAXWAIT 50

ServerMediaFile::ServerMediaFile(Config *c,MediaPlayerRegister *distributor):MediaFile(MPROVIDERID_SERVERMEDIAFILE){
  cfg=c;
  distributor->registerMediaProvider(this,MPROVIDERID_SERVERMEDIAFILE);
  dirhandler=new MediaLauncher(cfg);
  dirhandler->init();
  for (int i=0;i<NUMCHANNELS;i++) {
    launchers[i]=new MediaLauncher(cfg);
    launchers[i]->init(dirhandler);
  }
}

ServerMediaFile::~ServerMediaFile(){
  for (int i=0;i<NUMCHANNELS;i++) {
    launchers[i]->closeStream();
    delete launchers[i];
  }
  delete dirhandler;
  }

MediaList* ServerMediaFile::getRootList() {
  Log::getInstance()->log("MediaFile::getRootList",Log::DEBUG,"");
  MediaURI *ru=new MediaURI(providerid,NULL,NULL);
  MediaList *rt=new MediaList(ru);
  delete ru;
  //the configured Media List
  //for the moment up to 50 entries [Media] Dir.1 ...Dir.10
  struct stat st;
  for (int nr=1;nr<=50;nr++){
    char buffer[30];
    sprintf(buffer,"Dir.%d",nr);
    const char * dn=cfg->getValueString("Media",buffer);
    if (dn != NULL) {
      if (stat(dn,&st) != 0 || ! S_ISDIR(st.st_mode)) {
        Log::getInstance()->log("MediaFile::getRootList",Log::ERR,"unable to open basedir %s",dn);
      }
      else {
        Media *m=new Media();
        m->setFileName(dn);
        m->setMediaType(MEDIA_TYPE_DIR);
        m->setTime(st.st_mtime);
        sprintf(buffer,"Dir.Name.%d",nr);
        m->setDisplayName(cfg->getValueString("Media",buffer));
        rt->push_back(m);
        Log::getInstance()->log("Media",Log::DEBUG,"added base dir %s",dn);
      }
     }
   }
  return rt;
}

ULONG ServerMediaFile::getMediaType(const char *filename) {
  ULONG rt=dirhandler->getTypeForName(filename);
  if (rt != MEDIA_TYPE_UNKNOWN) return rt;
  return MediaFile::getMediaType(filename);
}

int ServerMediaFile::openMedium(ULONG channel, const MediaURI * uri, ULLONG * size, ULONG xsize, ULONG ysize){
  if (channel >= NUMCHANNELS) return -1;
  launchers[channel]->closeStream();
  ULONG rt=launchers[channel]->getTypeForName(uri->getName());
  if (rt != MEDIA_TYPE_UNKNOWN) {
    *size=0;
    channels[channel].reset();
    int rtstat=launchers[channel]->openStream(uri->getName(),xsize,ysize);
    channels[channel].setFilename(uri->getName());
    return rtstat>=0?0:1;
  }
  return MediaFile::openMedium(channel,uri,size,xsize,ysize);
}
    
int ServerMediaFile::getMediaBlock(ULONG channel, ULLONG offset, ULONG len, ULONG * outlen,
        unsigned char ** buffer)
{
  if (channel >= NUMCHANNELS) return -1;
  if (launchers[channel]->isOpen()) {
    return launchers[channel]->getNextBlock(len,buffer,outlen);
  }
  return MediaFile::getMediaBlock(channel,offset,len,outlen,buffer);
}
int ServerMediaFile::closeMediaChannel(ULONG channel){
  if (channel >= NUMCHANNELS) return -1;
  if (launchers[channel]->isOpen()) {
    return launchers[channel]->closeStream();
  }
  return MediaFile::closeMediaChannel(channel);
}
int ServerMediaFile::getMediaInfo(ULONG channel, MediaInfo * result){
  if (channel >= NUMCHANNELS) return -1;
  if (launchers[channel]->isOpen()) {
    result->canPosition=false;
    result->type=launchers[channel]->getTypeForName(channels[channel].filename);
    return 0;
  }
  return MediaFile::getMediaInfo(channel,result);
}


MediaList* ServerMediaFile::getMediaList(const MediaURI *parent) {
  ULONG rt=dirhandler->getTypeForName(parent->getName());
  if (rt == MEDIA_TYPE_UNKNOWN) return MediaFile::getMediaList(parent);
  int op=dirhandler->openStream(parent->getName(),0,0);
  if (op < 0) {
    Log::getInstance()->log("Media",Log::ERR,"unable to open handler for %s",parent->getName());
    dirhandler->closeStream();
    return NULL;
  }
  int maxtries=0;
  unsigned char *inbuf=NULL;
  unsigned char linebuf[2*BUFSIZE];
  unsigned char *wrp=linebuf;
  ULONG outlen=0;
  MediaList *ml=new MediaList(parent);
  while (maxtries < MAXWAIT) {
    int ert=dirhandler->getNextBlock(BUFSIZE,&inbuf,&outlen);
    if (ert != 0) break;
    if (outlen == 0) {
      if (inbuf != NULL) free(inbuf);
      inbuf=NULL;
      maxtries++;
      continue;
    }
    maxtries=0;
    if (outlen > BUFSIZE) {
      Log::getInstance()->log("Media",Log::ERR,"invalid read len %llu in getBlock for list %s",outlen,parent->getName());
      free(inbuf);
      inbuf=NULL;
      continue;
    }
    memcpy(wrp,inbuf,outlen);
    free(inbuf);
    inbuf=NULL;
    ULONG handledBytes=addDataToList(linebuf,outlen+(wrp-linebuf),ml,true);
    memcpy(linebuf,wrp+outlen-handledBytes,wrp-linebuf+outlen-handledBytes);
    if (handledBytes > outlen) wrp-=handledBytes-outlen;
    if (wrp >= linebuf+BUFSIZE) {
        Log::getInstance()->log("Media",Log::ERR,"line to long in getBlock for list %s",parent->getName());
        wrp=linebuf;
    }
  }
  dirhandler->closeStream();
  return ml;
}


ULONG ServerMediaFile::addDataToList(unsigned char * buf, ULONG buflen,MediaList *list,bool extendedFormat) {
  ULONG handledBytes=0;
  char entrybuf[BUFSIZE+1];
  char display[BUFSIZE+1];
  int ebpos=0;
  while (handledBytes < buflen) {
    unsigned char c=buf[handledBytes];
    if (c == '\n') {
      entrybuf[ebpos]=0;
      /* complete line */
      /* handle # lines */
      for (int i=0;i<ebpos;i++) {
        if (entrybuf[i] == '#') {
          entrybuf[i]=0;
          break;
        }
        if (entrybuf[i] != ' ') break;
      }
      if (strlen(entrybuf) > 0) {
        Log::getInstance()->log("Media",Log::DEBUG,"handle list line %s",entrybuf);
        char *uriptr=entrybuf;
        if (extendedFormat) {
          /* search for a delimiter */
          while (*uriptr != ';' && *uriptr != 0) uriptr++;
          if (*uriptr == ';') uriptr++;
          if (*uriptr == 0) uriptr=entrybuf;
        }
        ULONG mt=getMediaType(uriptr);
        if (mt != MEDIA_TYPE_UNKNOWN) {
          if (uriptr != entrybuf) {
            memcpy(display,entrybuf,uriptr-entrybuf-1);
            display[uriptr-entrybuf-1]=0;
          }
          else {
            int i=strlen(entrybuf)-1;
            int len=i+1;
            for(;i>=0;i--) {
              if (entrybuf[i]=='/') break;
            }
            i++;
            if (entrybuf[i] != 0) {
              memcpy(display,&entrybuf[i],len-i);
              display[len-i]=0;
            }
            else {
              memcpy(display,entrybuf,len);
              display[len]=0;
            }
          }
          Media *m=new Media();
          MediaURI *u=NULL;
          if (*uriptr != '/') {
            /* add the directory of the list in front */
            MediaURI *p=list->getParent(list->getRoot());
            char ubuf[strlen(p->getName())+strlen(uriptr)+2];
            sprintf(ubuf,"%s/%s",p->getName(),uriptr);
            u=new MediaURI(providerid,ubuf,display);
            delete p;
          }
          else {
            u=new MediaURI(providerid,uriptr,display);
          }
          m->setFileName(display);
          m->setURI(u);
          delete u;
          m->setMediaType(mt);
          list->push_back(m);
          Log::getInstance()->log("Media",Log::DEBUG,"added media display %s, type %lu",display,mt);
        }
      }


      //do handling
      ebpos=0;
    }
    else if (c != '\r') {
      entrybuf[ebpos]=c;
      ebpos++;
      if (ebpos >= BUFSIZE) {
        /* line too long - ignore */
        Log::getInstance()->log("Media",Log::ERR,"line to long in add data");
        ebpos=0;
      }
    }
    handledBytes++;
  }
  return handledBytes-ebpos;
}
