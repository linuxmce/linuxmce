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

#include "media.h"
#include <time.h>
#include <arpa/inet.h>
#ifndef SNPRINTF
#define SNPRINTF snprintf
#endif


MediaURI::MediaURI(const MediaURI *cp) {
  if ( ! cp) {
    _providerId=0;
    _name=NULL;
    _display=NULL;
    _allowedTypes=MEDIA_TYPE_ALL;
  }
  else {
    _providerId=cp->_providerId;
    _allowedTypes=cp->_allowedTypes;
    _name=NULL;
    if (cp->_name) {
      _name=new char[strlen(cp->_name)+1];
      strcpy(_name,cp->_name);
    }
    _display=NULL;
    if (cp->_display) {
      _display=new char[strlen(cp->_display)+1];
      strcpy(_display,cp->_display);
    }
  }
} 
MediaURI::MediaURI(ULONG p, const char * n, const char * dp) {
  _allowedTypes=MEDIA_TYPE_ALL;
  _providerId=p;
  if (n) {
    _name=new char[strlen(n)+1];
    strcpy(_name,n);
  } else {
    _name=NULL;
  }
  _display=NULL;
  if (dp) {
    _display=new char[strlen(dp)+1];
    strcpy(_display,dp);
  }
}


int MediaURI::getSerializedLenImpl() {
  int rt=4+4; //provider+allowedType
  rt+=getSerializedStringLen(_name);
  rt+=getSerializedStringLen(_display);
  return rt;
}
/**
  * 4 provider
  * 4 allowedType
  * 4 namelen (incl. 0)
  * name+0
  * 4 displaylen (incl. 0)
  * display+0
  */
int MediaURI::serializeImpl(SerializeBuffer *b) {
  version=1;
  if (b->encodeLong(_providerId) != 0) return -1; 
  if (b->encodeLong(_allowedTypes) != 0) return -1; 
  if (b->encodeString(_name) != 0) return -1; 
  if (b->encodeString(_display) != 0) return -1; 
  return 0;
}

int MediaURI::deserializeImpl(SerializeBuffer *b) {
  if (_name) delete _name;
  _name=NULL;
  if (_display) delete _display;
  _display=NULL;
  if (b->decodeLong(_providerId) != 0) return -1;
  if (b->decodeLong(_allowedTypes) != 0) return -1;
  ULONG nlen=0;
  if (b->decodeString(nlen,_name) != 0) return -1;
  if (b->decodeString(nlen,_display) != 0) return -1;
  //if (version > 1) ...
  return 0;
}




int MediaInfo::getSerializedLenImpl() {
  int rt=8+1+4+4; //8len+1canPos+4type+4subtype
  return rt;
}


/**
  * serialize to buffer
  * 8 len
  * 1 canPos
  * 4 type
  * 4 subtype
  */
int MediaInfo::serializeImpl(SerializeBuffer *b) {
  if (b->encodeLongLong(size) != 0) return -1;
  if (b->encodeByte(canPosition?1:0) != 0) return -1;
  if (b->encodeLong(type) != 0) return -1; 
  if (b->encodeLong(subtype) != 0) return -1;
  return 0;
}
/**
  * deserialize
  * should be compatible to older serialize functions
  */
int MediaInfo::deserializeImpl(SerializeBuffer *b) {
  if (b->decodeLongLong(size) != 0) return -1;
  UCHAR cp=0;
  if (b->decodeByte(cp) != 0) return -1;
  canPosition=cp!=0;
  if (b->decodeLong(type) != 0) return -1;
  if (b->decodeLong(subtype) != 0) return -1;
  return 0;
}



Media::Media()
{
  mtime = 0;
  displayName = NULL;
  fileName = NULL;
  mediaType=MEDIA_TYPE_UNKNOWN;
  uri=NULL;
  index=0;
}

Media::Media(const Media *m) {
  Media();
  if (! m) return;
  mtime=m->mtime;
  mediaType=m->mediaType;
  uri=NULL;
  fileName=NULL;
  displayName=NULL;
  if (m->uri) uri=new MediaURI(m->uri);
  setFileName(m->fileName);
  setDisplayName(m->displayName);
  index=m->index;
}

Media::~Media()
{
  if (displayName) { delete[] displayName; displayName = NULL; }
  if (fileName) { delete[] fileName; fileName = NULL; }
  if (uri) delete uri;
  index = -1; // just in case
}

ULONG Media::getTime() const
{
  return mtime;
}

const char* Media::getDisplayName() const
{
  if (displayName) return displayName;
  return fileName;
}

const char* Media::getFileName() const
{
  return fileName;
}

void Media::setTime(ULONG tstartTime)
{
  mtime = tstartTime;
}

void Media::setMediaType(ULONG mtype)
{
  mediaType=mtype;
}

ULONG Media::getMediaType() const
{
  return mediaType;
}

void Media::setDisplayName(const char* tDisplayName)
{
  if (displayName) delete[] displayName;
  displayName=NULL;
  if (! tDisplayName) return;
  displayName = new char[strlen(tDisplayName) + 1];
  if (displayName) strcpy(displayName, tDisplayName);
}

void Media::setFileName(const char* tFileName)
{
  if (fileName) delete[] fileName;
  fileName=NULL;
  if (! tFileName) return;
  fileName = new char[strlen(tFileName) + 1];
  if (fileName) strcpy(fileName, tFileName);
}

bool Media::hasDisplayName() const {
  return (displayName != NULL);
}

char * Media::getTimeString(char * buffer) const {
  if (! buffer) buffer=new char[TIMEBUFLEN];
  struct tm ltime;
  time_t tTime = (time_t)getTime();
  struct tm *btime = localtime(&tTime);
  memcpy(&ltime,btime, sizeof(struct tm));
  btime=&ltime;
  if (btime && tTime != 0) {
#ifndef _MSC_VER
  strftime(buffer,TIMEBUFLEN, "%0g/%0m/%0d %0H:%0M ", btime);
#else
  strftime(buffer, TIMEBUFLEN, "%g/%m/%d %H:%M ", btime);
#endif
  }
  else {
    SNPRINTF(buffer,TIMEBUFLEN,"00/00/00 00:00 ");
    }
  return buffer;
}

const MediaURI * Media::getURI() const {
  return uri;
}

void Media::setURI(const MediaURI *u) {
  if (uri) delete uri;
  uri=new MediaURI(u);
}

int Media::getSerializedLenImpl() {
  int rt=4+4+1; //type,time,hasURI
  rt+=getSerializedStringLen(fileName);
  rt+=getSerializedStringLen(displayName);
  if (uri) rt+=uri->getSerializedLen();
  return rt;
}
/**
  * 4 type
  * 4 time
  * 4 namelen (incl. 0)
  * name+0
  * 4 displaylen (incl. 0)
  * display+0
  * 1 hasURI
  * URI
  */
int Media::serializeImpl(SerializeBuffer *b) {
  if (b->encodeLong(mediaType) != 0) return -1; 
  if (b->encodeLong(mtime) != 0) return -1; 
  if (b->encodeString(fileName) != 0) return -1; 
  if (b->encodeString(displayName) != 0) return -1; 

  if (b->encodeByte(uri?1:0) != 0) return -1; 
  if (uri) {
    if (uri->serialize(b) != 0) return -1;
  }
  return 0;
}

int Media::deserializeImpl(SerializeBuffer *b) {
  if (fileName) delete fileName;
  fileName=NULL;
  if (displayName) delete displayName;
  displayName=NULL;
  if (uri) delete uri;
  uri=NULL;
  if (b->decodeLong(mediaType) != 0) return -1;
  if (b->decodeLong(mtime) != 0) return -1;
  ULONG nlen=0;
  if (b->decodeString(nlen,fileName) != 0) return -1;
  if (b->decodeString(nlen,displayName) != 0) return -1;
  UCHAR hasURI=0;
  if (b->decodeByte(hasURI) != 0) return -1;
  if (hasURI!=0) {
    uri=new MediaURI();
    if (uri->deserialize(b) != 0) return -1;
  }
  return 0;
}



MediaURI * MediaList::getURI(Media * m) {
  if (! m) return NULL;
  const MediaURI *rtc=m->getURI();
  if (rtc) return new MediaURI(rtc);
  if (!_root) return NULL;
  if (! m->getFileName()) return NULL;
  int len=strlen(m->getFileName());
  if (_root->getName()) len+=strlen(_root->getName())+1;
  MediaURI *rt=new MediaURI();
  rt->_name=new char[len+1];
  const char *fn=m->getFileName();
  if (_root->getName()) {
    while (*fn=='/') fn++;
    sprintf(rt->_name,"%s/%s",_root->getName(),fn);
  }
  else {
    sprintf(rt->_name,"%s",fn);
  }
  if (m->hasDisplayName() || _root->hasDisplayName()) {
    len=strlen(m->getDisplayName())+1;
    if (_root->hasDisplayName()) {
      len+=strlen(_root->getDisplayName())+2;
    }
    rt->_display=new char[len];
    if (_root->hasDisplayName()) {
      const char *sp=m->getDisplayName();
      if (*sp=='/')sp++;
      sprintf(rt->_display,"%s/%s",_root->getDisplayName(),sp);
    }
    else {
      sprintf(rt->_display,"%s",m->getDisplayName());
    }
  }
  rt->_providerId=_root->_providerId;
  rt->_allowedTypes=_root->_allowedTypes;
  return rt;
}

MediaURI * MediaList::getParent(MediaURI *c) {
  MediaURI * rt=new MediaURI();
  rt->_providerId=c->_providerId;
  rt->_allowedTypes=c->_allowedTypes;
  ULONG nlen=0;
  if (c->_name) {
    char * ls=strrchr(c->_name,'/');
    if (ls) {
      nlen=ls-c->_name;
      rt->_name=new char[nlen+1];
      strncpy(rt->_name,c->_name,nlen);
      rt->_name[nlen]=0;
    }
  }
  if (c->_display) {
    char * ls=strrchr(c->_display,'/');
    if (ls) {
      nlen=ls-c->_display;
      rt->_display=new char[nlen+1];
      strncpy(rt->_display,c->_display,nlen);
      rt->_display[nlen]=0;
    }
  }
  return rt;
}


MediaList::MediaList(const MediaURI *root) {
  _root=new MediaURI(root);
  _owning=true;
}


MediaList::~MediaList() {
  emptyList();
}
void MediaList::emptyList(){
  if (_owning) {
    for (UINT i = 0; i < size(); i++)
      {
        delete (*this)[i];
      }
  }
  clear();
  if (_root) delete _root;
  _root=NULL;
  _owning=true;
}


MediaURI * MediaList::getRootURI() {
  if ( ! _root) return NULL;
  return new MediaURI(_root);
}

ULONG MediaList::getProvider() {
  if (! _root) return 0;
  return _root->getProvider();
}

void MediaList::setOwning(bool owning) {
  _owning=owning;
}


  
int MediaList::getSerializedLenImpl() {
  int rt=4+1; //numelem+hasRoot
  if (_root) rt+=_root->getSerializedLen();
  for (MediaList::iterator it=begin();it<end();it++) {
    rt+=(*it)->getSerializedLen();
  }
  return rt;
}

/**
  * 4 numelem
  * 1 hasRoot
  * URI root
  * nx Media elements
  * 
  */
int MediaList::serializeImpl(SerializeBuffer *b) {
  if (b->encodeLong(size()) != 0) return -1; 
  if (b->encodeByte(_root?1:0) != 0) return -1; 
  if (_root) {
    if (_root->serialize(b) != 0) return -1;
  }
  for (MediaList::iterator it=begin();it<end();it++) {
    if ((*it)->serialize(b) !=0) return -1;
  }
  return 0;
}

int MediaList::deserializeImpl(SerializeBuffer *b) {
  emptyList();
  ULONG numelem;
  if (b->decodeLong(numelem) != 0) return -1;
  UCHAR hasRoot=0;
  if (b->decodeByte(hasRoot) != 0) return -1;
  if (hasRoot!=0) {
    _root=new MediaURI();
    if (_root->deserialize(b) != 0) return -1;
  }
  for (ULONG i=0;i<numelem;i++) {
    Media *m=new Media();
    if (m->deserialize(b) != 0) {
      delete m;
      return -1;
    }
    push_back(m);
  }
  return 0;
}



