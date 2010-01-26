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

#include "serialize.h"
#include <stdlib.h>
#include <arpa/inet.h>
#ifndef SNPRINTF
#define SNPRINTF snprintf
#endif

#define BUFFERINCREASE 1024

int SerializeBuffer::seek(int amount) {
  UCHAR *np=current+amount;
  if (np < start || np > end) return -1;
  current=np;
  return 0;
}

void SerializeBuffer::rewind() {
  current=start;
}

int SerializeBuffer::checkSpace(int amount) {
  if ((current+amount) <= end) return 0;
  if (owning && autoincrease) {
     if (start+size > current+amount) {
       end=start+size;
       return 0;
     }
     UCHAR *ns=new UCHAR[size+BUFFERINCREASE];
     if (!ns) return -1;
     memcpy(ns,start,end-start);
     size=size+BUFFERINCREASE;
     end=ns+size;
     if (useMalloc) free( start);
     else delete [] start;
     start=ns;
     return 0;
  }
  return -1;
}

SerializeBuffer::~SerializeBuffer() {
  if (owning) {
    if (useMalloc) free(start);
    else delete[] start;
    start=NULL;
  }
}
SerializeBuffer::SerializeBuffer(ULONG sz,bool isMalloc,bool ai){
  autoincrease=ai;
  useMalloc=isMalloc;
  if (isMalloc) {
    start=(UCHAR *)malloc(sz);
  }
  else {
    start=new UCHAR[sz];
  }
  end=start;
  current=start;
  size=sz;
  owning=true;
}
    //constructor for SerializeBuffers with external buffers
SerializeBuffer::SerializeBuffer(UCHAR *buffer,ULONG sz,bool ow,bool isMalloc,bool ai) {
  useMalloc=isMalloc;
  autoincrease=ai;
  owning=ow;
  size=sz;
  start=buffer;
  current=start;
  end=start+size;
}
/**
  * helper for serialize and deserialize
  * always return != if no space
  * advance buffer pointer
  */

int SerializeBuffer::encodeLong(ULONG data) {
  if (checkSpace( (int)sizeof(ULONG))!=0) return -1;
  *((ULONG *)(current))=htonl(data); 
  current+=sizeof(ULONG);
  return 0;
}
int SerializeBuffer::encodeShort(USHORT data) {
  if (checkSpace( (int)sizeof(USHORT))!=0) return -1;
  *((USHORT *)(current))=htons(data); 
  current+=sizeof(USHORT);
  return 0;
}
int SerializeBuffer::encodeByte(UCHAR data) {
  if (checkSpace( (int)sizeof(UCHAR))!=0) return -1;
  *((UCHAR *)(current))=data; 
  current+=sizeof(UCHAR);
  return 0;
}
int SerializeBuffer::encodeLongLong(ULLONG data) {
  if (checkSpace( (int)sizeof(ULLONG))!=0) return -1;
  *((ULONG *)(current))=htonl((data>>32) & 0xffffffff); 
  current+=sizeof(ULONG);
  *((ULONG *)(current))=htonl(data & 0xffffffff); 
  current+=sizeof(ULONG);
  return 0;
}
//string: 4 len, string with 0
int SerializeBuffer::encodeString(const char *str) {
  if (checkSpace( (int)sizeof(ULONG))!=0) return -1;
  ULONG len=0;
  if (str) len=strlen(str)+1;
  *((ULONG *)(current))=htonl(len); 
  current+=sizeof(ULONG);
  if (len == 0) return 0;
  if (checkSpace((int)len)!=0) return -1;
  strcpy((char *) current,str);
  current+=len;
  return 0;
}
int SerializeBuffer::decodeLong( int &data) {
  if (checkSpace( (int)sizeof(ULONG))!=0) return -1;
  data=(int)ntohl(*((ULONG *)(current))); 
  current+=sizeof(ULONG);
  return 0;
}
int SerializeBuffer::decodeLong(ULONG &data) {
  if (checkSpace( (int)sizeof(ULONG))!=0) return -1;
  data=ntohl(*((ULONG *)(current))); 
  current+=sizeof(ULONG);
  return 0;
}
int SerializeBuffer::decodeShort(USHORT &data) {
  if (checkSpace( (int)sizeof(USHORT))!=0) return -1;
  data=ntohs(*((USHORT *)(current))); 
  current+=sizeof(USHORT);
  return 0;
}
int SerializeBuffer::decodeByte(UCHAR &data) {
  if (checkSpace( (int)sizeof(UCHAR))!=0) return -1;
  data=*((UCHAR *)current);
  current+=sizeof(UCHAR);
  return 0;
}
int SerializeBuffer::decodeLongLong(ULLONG &data) {
  if (checkSpace( (int)sizeof(ULLONG))!=0) return -1;
  ULLONG hd=ntohl(*((ULONG *)(current))); 
  current+=sizeof(ULONG);
  ULLONG ld=ntohl(*((ULONG *)(current))); 
  current+=sizeof(ULONG);
  data=(hd << 32) | ld;
  return 0;
}
//string: 4 len, string with 0
int SerializeBuffer::decodeString(ULONG &len, char *&strbuf) {
  strbuf=NULL;
  len=0;
  if (checkSpace( (int)sizeof(ULONG))!=0) return -1;
  len=ntohl(*((ULONG *)(current))); 
  current+=sizeof(ULONG);
  if (len == 0) return 0;
  if (checkSpace((int)len)!=0) return -1;
  strbuf=new char[len];
  strncpy(strbuf,(char *)current,len);
  *(strbuf+len-1)=0;
  current+=len;
  return 0;
}


UCHAR * SerializeBuffer::steelBuffer() {
  UCHAR *rt=start;
  owning=false;
  autoincrease=false;
  return rt;
}

int Serializable::getSerializedStringLen(const char * str) {
  int rt=4;
  if (str) rt+=strlen(str)+1;
  return rt;
}

USHORT Serializable::getVersion() {
  return version;
}


Serializable::Serializable() {
  version=1;
}
Serializable::~Serializable(){}

int Serializable::getSerializedLen() {
  //2version+4len
  return 6 + getSerializedLenImpl();
}
int Serializable::serialize(SerializeBuffer *b) {
  UCHAR *ptr=b->getCurrent();
  if (b->encodeShort(version) != 0) return -1;
  if (b->encodeLong(0) != 0) return -1; //dummy len
  if (serializeImpl(b) != 0) return -1;
  UCHAR *ep=b->getCurrent();
  //now write the len
  int len=ep-ptr-6;
  if (len < 0) return -1 ; //internal error
  b->seek(ptr-ep+2); //to len field
  if (b->encodeLong(len) != 0) return -1;
  b->seek(ep-b->getCurrent()); //back to end
  return 0;
}

int Serializable::deserialize(SerializeBuffer *b) {
  USHORT vers=0;
  if (b->decodeShort(vers) != 0) return -1;
  ULONG len=0;
  if (b->decodeLong(len) != 0) return -1;
  UCHAR *data=b->getCurrent();
  if (data+len > b->getEnd()) return -1;
  //TODO: set end temp. to current+len
  //for better error handling in deserializeImpl
  if (deserializeImpl(b) != 0) return -1;
  //ensure we go to end of this element regardless of the things we know
  b->seek(data+len-b->getCurrent());
  return 0;
}
  
SerializableList::SerializableList() {
  version=1;
  encodeOnly=false;
}
SerializableList::~SerializableList(){}

int SerializableList::addParam(Serializable *p,USHORT v) {
  if (v < version || p == NULL) return -1;
  Pentry entry;
  entry.ptype=TSER;
  entry.ptr.pser=p;
  entry.version=v;
  list.push_back(entry);
  version=v;
  return 0;
}
int SerializableList::addParam(USHORT *p,USHORT v) {
  if (v < version || p == NULL) return -1;
  Pentry entry;
  entry.ptype=TUSHORT;
  entry.ptr.pshort=p;
  entry.version=v;
  list.push_back(entry);
  version=v;
  return 0;
}
int SerializableList::addParam(ULONG *p,USHORT v) {
  if (v < version || p == NULL) return -1;
  Pentry entry;
  entry.ptype=TULONG;
  entry.ptr.plong=p;
  entry.version=v;
  list.push_back(entry);
  version=v;
  return 0;
}
int SerializableList::addParam(ULLONG *p,USHORT v) {
  if (v < version || p == NULL) return -1;
  Pentry entry;
  entry.ptype=TULLONG;
  entry.ptr.pllong=p;
  entry.version=v;
  list.push_back(entry);
  version=v;
  return 0;
}
int SerializableList::addParam(char **p,USHORT v) {
  if (v < version || p == NULL) return -1;
  Pentry entry;
  entry.ptype=TCHAR;
  entry.ptr.pchar=p;
  entry.version=v;
  list.push_back(entry);
  version=v;
  return 0;
}

bool SerializableList::Pentry::isEqual(void *p,SerializableList::Ptypes t) {
  void *cmp=NULL;
  switch(t) {
    case TUSHORT:
      cmp=(void *)ptr.pshort;
      break;
    case TULONG:
      cmp=(void *)ptr.plong;
      break;
    case TULLONG:
      cmp=(void *)ptr.pllong;
      break;
    case TSER:
      cmp=(void *)ptr.pser;
      break;
    case TCHAR:
      cmp=(void *)ptr.pchar;
      break;
    case TUNKNOWN:
      break;
  }
  return p==cmp;
}

SerializableList::Pentry *SerializableList::findEntry(void *p,SerializableList::Ptypes t) {
  for (vector<Pentry>::iterator it=list.begin();it<list.end();it++) {
    if ( (*it).isEqual(p,t)) return &(*it);
  }
  return NULL;
}
bool SerializableList::isDeserialized(Serializable *p){
  SerializableList::Pentry *e=findEntry(p,TSER);
  if (!e) return false;
  return e->isDeserialized;
}
bool SerializableList::isDeserialized(USHORT *p){
  SerializableList::Pentry *e=findEntry(p,TUSHORT);
  if (!e) return false;
  return e->isDeserialized;
}
bool SerializableList::isDeserialized(ULONG *p){
  SerializableList::Pentry *e=findEntry(p,TULONG);
  if (!e) return false;
  return e->isDeserialized;
}

bool SerializableList::isDeserialized(ULLONG *p){
  SerializableList::Pentry *e=findEntry(p,TULLONG);
  if (!e) return false;
  return e->isDeserialized;
}

bool SerializableList::isDeserialized(char **p){
  SerializableList::Pentry *e=findEntry(p,TCHAR);
  if (!e) return false;
  return e->isDeserialized;
}

int SerializableList::getSerializedLenImpl(){
  int rt=0;
  for (vector<Pentry>::iterator it=list.begin();it<list.end();it++) {
    switch((*it).ptype){
      case TUSHORT:
        rt+=sizeof(USHORT);
        break;
      case TULONG:
        rt+=sizeof(ULONG);
        break;
      case TULLONG:
        rt+=sizeof(ULLONG);
        break;
      case TCHAR:
        rt+=getSerializedStringLen(*((*it).ptr.pchar));
        break;
      case TSER:
        rt+=(*it).ptr.pser->getSerializedLen();
        break;
      case TUNKNOWN:
        break;
    }
  }
  return rt;
}

int SerializableList::serializeImpl(SerializeBuffer *b){
  for (vector<Pentry>::iterator it=list.begin();it<list.end();it++) {
    switch((*it).ptype){
      case TUSHORT:
        if (b->encodeShort(*(*it).ptr.pshort) != 0) return -1;
        break;
      case TULONG:
        if (b->encodeLong(*(*it).ptr.plong) != 0) return -1;
        break;
      case TULLONG:
        if (b->encodeLongLong(*(*it).ptr.pllong) != 0) return -1;
        break;
      case TCHAR:
        if (b->encodeString(*(*it).ptr.pchar) != 0) return -1;
        break;
      case TSER:
        if ((*it).ptr.pser->serialize(b) != 0) return -1;
        break;
      case TUNKNOWN:
        break;
    }
  }
  return 0;
}

int SerializableList::deserializeImpl(SerializeBuffer *b){
  ULONG dlen=0;
  for (vector<Pentry>::iterator it=list.begin();it<list.end();it++) {
    if ((*it).version > version) {
      //OK - we received an older version - stop here
      break;
    }
    switch((*it).ptype){
      case TUSHORT:
        if (b->decodeShort(*(*it).ptr.pshort) != 0) return -1;
        break;
      case TULONG:
        if (b->decodeLong(*(*it).ptr.plong) != 0) return -1;
        break;
      case TULLONG:
        if (b->decodeLongLong(*(*it).ptr.pllong) != 0) return -1;
        break;
      case TCHAR:
        if (b->decodeString(dlen,*(*it).ptr.pchar) != 0) return -1;
        break;
      case TSER:
        if ((*it).ptr.pser->deserialize(b) != 0) return -1;
        break;
      case TUNKNOWN:
        break;
    }
    (*it).isDeserialized=true;
  }
  return 0;
}


