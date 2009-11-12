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

#include "mediafile.h"
#include "media.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include "log.h"




MediaFile::MediaFile(ULONG pid){
  providerid=pid;
}

MediaFile::~MediaFile(){
  };



static struct mtype{
   const char* extension;
   ULONG type;
   } mediatypes[]= {
     {".mp3",MEDIA_TYPE_AUDIO},
     {".MP3",MEDIA_TYPE_AUDIO},
     {".jpg",MEDIA_TYPE_PICTURE},
     {".JPG",MEDIA_TYPE_PICTURE},
     {".jpeg",MEDIA_TYPE_PICTURE},
     {".JPEG",MEDIA_TYPE_PICTURE},
     {".mpg",MEDIA_TYPE_VIDEO},
     {".MPG",MEDIA_TYPE_VIDEO}
     };
//#define NUMTYPES (sizeof(mediatypes)/sizeof(mtype))
#define NUMTYPES 8

//helper from vdr tools.c
bool endswith(const char *s, const char *p)
{
  const char *se = s + strlen(s) - 1;
  const char *pe = p + strlen(p) - 1;
  while (pe >= p) {
        if (*pe-- != *se-- || (se < s && pe >= p))
           return false;
        }
  return true;
}

MediaList* MediaFile::getRootList() {
  //has to be implemented in the derived class
  return NULL;
}

ULONG MediaFile::getMediaType(const char * filename) {
 for (ULONG i=0;i<NUMTYPES;i++) {
      if (endswith(filename,mediatypes[i].extension)) {
        return mediatypes[i].type;
        }
     }
 return MEDIA_TYPE_UNKNOWN;
}


Media * MediaFile::createMedia(const char * dirname, const char * filename, bool withURI) {
  Media * rt=NULL;
  char *buffer;
  asprintf(&buffer, "%s/%s", dirname, filename);
  struct stat st;
  ULONG mtype=MEDIA_TYPE_UNKNOWN;
  if (stat(buffer, &st) == 0) {
    if (S_ISDIR(st.st_mode)) {
       mtype=MEDIA_TYPE_DIR;
    }
    else {
       mtype=getMediaType(filename);
    }
  }
  //only consider entries we accept by type here
  if (mtype != MEDIA_TYPE_UNKNOWN) {
    rt =new Media();
    rt->setMediaType(mtype);
    rt->setFileName(filename);
    rt->setTime(st.st_mtime);
    Log::getInstance()->log("Media",Log::DEBUG,"created Media %s, type=%d",filename,mtype);
    if (withURI) {
      MediaURI u(providerid,buffer,NULL);
      rt->setURI(&u);
    }
  }
  free(buffer);
  return rt;
}

MediaList* MediaFile::getMediaList(const MediaURI * parent){
  ULONG mediaType=parent->getAllowedTypes();
  Log::getInstance()->log("MediaFile::getMediaList",Log::DEBUG,"parent %s,types=0x%0lx",parent->getName(),mediaType);
  MediaList *rt=NULL;
  rt=new MediaList(parent);
  const char *dirname=parent->getName();
  //open the directory and read out the entries
  DIR *d=opendir(dirname);
  struct dirent *e;
  union { // according to "The GNU C Library Reference Manual"
    struct dirent d;
    char b[offsetof(struct dirent, d_name) + NAME_MAX + 1];
    } u;

  while (d != NULL && (readdir_r(d,&u.d,&e) == 0) && e != NULL) {
    const char * fname=e->d_name;
    if ( fname == NULL) continue;
    if (strcmp(fname,".") == 0) continue;
    if (strcmp(fname,"..") == 0) continue;
    Media *m=createMedia(dirname,fname);
    if (m && ( m->getMediaType() & mediaType)) {
      Log::getInstance()->log("Media",Log::DEBUG,"added entry %s, type=%d",fname,m->getMediaType());
      rt->push_back(m);
    }
    else {
      if (m) delete m;
    }
  }
  if (d != NULL) closedir(d);
  return rt;
  }


int MediaFile::openMedium(ULONG channel, const MediaURI * uri, ULLONG * size, ULONG xsize, ULONG ysize) {
  Log::getInstance()->log("Media::openMedium",Log::DEBUG,"fn=%s,chan=%u",uri->getName(),channel);
  *size=0;
  if (channel <0 || channel >= NUMCHANNELS) return -1;
  struct ChannelInfo *info=&channels[channel];
  if (info->file) info->reset();
  FILE *fp=fopen(uri->getName(),"r");
  if (! fp) {
    Log::getInstance()->log("Media::openMedium",Log::ERR,"unable to open file fn=%s,chan=%u",uri->getName(),channel);
    return -1;
  }
  struct stat st;
  ULLONG mysize=0;
  if ( fstat(fileno(fp),&st) == 0) mysize=st.st_size;
  if (mysize == 0) {
    Log::getInstance()->log("Media::openMedium",Log::ERR,"unable to open file fn=%s,chan=%u",uri->getName(),channel);
    fclose(fp);
    return -1;
  }
  info->setFilename(uri->getName());
  info->file=fp;
  info->size=mysize;
  *size=mysize;
  info->provider=providerid;
  return 0;
}


int MediaFile::getMediaBlock(ULONG channel, ULLONG offset, ULONG len, ULONG * outlen,
        unsigned char ** buffer) {
  Log::getInstance()->log("Media::getMediaBlock",Log::DEBUG,"chan=%u,offset=%llu,len=%lu",channel,offset,len);
  *outlen=0;
  if (channel <0 || channel >= NUMCHANNELS) return -1;
  struct ChannelInfo *info=&channels[channel];
  if (! info->file) {
    Log::getInstance()->log("Media::getMediaBlock",Log::ERR,"not open chan=%u",channel);
    return -1;
  }
  ULLONG cpos=ftell(info->file);
  if (offset != cpos) {
    fseek(info->file,offset-cpos,SEEK_CUR);
  }
  if (offset != (ULLONG)ftell(info->file)) {
    Log::getInstance()->log("Client", Log::DEBUG, "getMediaBlock pos = %llu not available", offset);
    return -1;
  }
  if (*buffer == NULL) *buffer=(UCHAR *)malloc(len);
  if (*buffer == NULL) {
    Log::getInstance()->log("Media::getMediaBlock",Log::ERR,"uanble to allocate buffer");
    return -1;
  }
  ULONG amount=fread(*buffer,1,len,info->file);
  Log::getInstance()->log("Media::getMediaBlock",Log::DEBUG,"readlen=%lu",amount);
  *outlen=amount;
  return 0;
}


int MediaFile::closeMediaChannel(ULONG channel){
  Log::getInstance()->log("Media::closeMediaChannel",Log::DEBUG,"chan=%u",channel);
  if (channel <0 || channel >= NUMCHANNELS) return -1;
  struct ChannelInfo *info=&channels[channel];
  info->reset();
  return 0;
}

//TODO: fill in more info
int MediaFile::getMediaInfo(ULONG channel, MediaInfo * result){
  Log::getInstance()->log("Media::getMediaInfo",Log::DEBUG,"chan=%u",channel);
  if (channel <0 || channel >= NUMCHANNELS) return -1;
  struct ChannelInfo *info=&channels[channel];
  if (! info->file) return -1;
  result->size=info->size;
  result->canPosition=true;
  return 0;
}
