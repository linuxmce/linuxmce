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

#ifndef MEDIA_H
#define MEDIA_H

using namespace std;
#include <vector>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "serialize.h"

/* media types form a bitmask
   so you can add them to have > 1*/
#define MEDIA_TYPE_DIR 1
#define MEDIA_TYPE_AUDIO 2
#define MEDIA_TYPE_VIDEO 4
#define MEDIA_TYPE_PICTURE 8
#define MEDIA_TYPE_UNKNOWN 0

#define MEDIA_TYPE_ALL (1+2+4+8)

/**
  * MediaURI - a data holder for the complete path to a media
  * depending on the provider there is an internal name and a display name
  * by providing own MediaList implementations the provider can control
  * how URIs are concatenated
  */
class MediaURI : public Serializable{
  //to be able to access private members
  friend class MediaList;
  private:
    char * _name;
    char * _display;
    ULONG _providerId;
    ULONG _allowedTypes;
  public:
    MediaURI() {
      _name=NULL;
      _display=NULL;
      _providerId=0;
      _allowedTypes=MEDIA_TYPE_ALL;
    }
    //constructor copying params
    MediaURI(ULONG provider, const char * name, const char * display);
    virtual ~MediaURI() {
      if (_name) delete _name;
      if (_display) delete _display;
    }
    MediaURI(const MediaURI *cp) ;
    const char * getName() const { return _name;}
    const char * getDisplayName() const { 
      if (_display) return _display;
      return _name;
    }
    ULONG getProvider() const {
      return _providerId;
    }
    void setProvider(ULONG pid) {
      _providerId=pid;
    }
    ULONG getAllowedTypes() const {
      return _allowedTypes;
    }
    void setAllowedTypes(ULONG allowedTypes) {
      _allowedTypes=allowedTypes;
    }
    bool hasDisplayName() const {
      return _display!=NULL;
    }

    //serialize functions
    //get the #of bytes needed to serialize
    virtual int getSerializedLenImpl();
    //serialize
    //advance buffer, check if >= end
    //return 0 if OK
    virtual int serializeImpl(SerializeBuffer *b);
    //deserialize
    //advance buffer, check if >= end
    //return 0 if OK
    virtual int deserializeImpl(SerializeBuffer *b);
 
};

/**
  * a class providing additional info for a medium
  */
class MediaInfo : public Serializable{
  public:
    ULLONG  size;
    bool                canPosition;
    ULONG                 type; //a media type
    ULONG                 subtype; //TODO
    /**
      * return any info item contained within this info
      */
    virtual const char * getInfo(ULONG infoId) { return NULL;}
    virtual ULLONG getIntegerInfo(ULONG infoId) { return 0;}
    virtual const char * getInfoName(ULONG infoId) { return NULL;}
    virtual bool hasInfo(ULONG infoId) { return false;}
    MediaInfo() {
      size=0;
      canPosition=true;
      type=MEDIA_TYPE_UNKNOWN;
      subtype=0;
    }
    virtual ~MediaInfo(){};
    //serialize functions
    //get the #of bytes needed to serialize
    virtual int getSerializedLenImpl();
    //serialize
    //advance buffer, check if >= end
    //return 0 if OK
    virtual int serializeImpl(SerializeBuffer *b);
    //deserialize
    //advance buffer, check if >= end
    //return 0 if OK
    virtual int deserializeImpl(SerializeBuffer *b);
};


/**
  * the Media class - a data holder describing a single media
  * WITHOUT the complete path
  * to retrieve an URI you need the list where this media is contained
  * this has the root URI and can construct the URI for this media
  * optional the media can contain an UIR by itself - then this is used
  */

class Media : public Serializable
{
  friend class MediaList;
  public:
    Media();
    Media(const Media *m);
    virtual ~Media();

    void setTime(ULONG mtimeTime);
    void setDisplayName(const char* displayName);
    void setFileName(const char* fileName);
    void setMediaType(ULONG mtype);

    ULONG getTime() const;
    const char* getDisplayName() const;
    const char* getFileName() const;
    //return the time as a string
    //if the user provides a buffer, this one is used, if NULL
    //is given a new buffer is allocated
    //caller must delete the buffer after usage!
    char * getTimeString(char *buffer) const;
    //length for the time display buffer
    const static int TIMEBUFLEN=100;
    int index;
    ULONG getMediaType() const;
    bool hasDisplayName() const;
    //optionally the media can contain an URI
    //in this case the filename is not considered
    //but the data from the URI is taken
    //this enables having another providerId set in the media...
    //returns URI without copy
    const MediaURI * getURI() const;
    void setURI(const MediaURI *uri);

    //serialize functions
    //get the #of bytes needed to serialize
    virtual int getSerializedLenImpl();
    //serialize
    //advance buffer, check if >= end
    //return 0 if OK
    virtual int serializeImpl(SerializeBuffer *b);
    //deserialize
    //advance buffer, check if >= end
    //return 0 if OK
    virtual int deserializeImpl(SerializeBuffer *b);
    
  private:
    ULONG mtime;
    char* displayName;
    char* fileName;
    ULONG mediaType;
    MediaURI *uri;


};


typedef vector<Media*> MediaListI;

/**
  * the MediaList - containing a root URI and
  * all the Media entries
  * providers can provide derived classes to overwrite the URI-Methods
  */
class MediaList : public MediaListI , public Serializable{
  private:
    MediaURI *_root;
    bool _owning;
    void emptyList();
  public:
    MediaList(const MediaURI *root); //root is copied
    virtual ~MediaList();
    //no copy root UIR
    virtual MediaURI * getRoot() {return _root;}
    //all methods return a copy of the URI
    //so the caller has to destroy this
    virtual MediaURI * getRootURI();
    virtual MediaURI * getParent(MediaURI *c) ;
    virtual MediaURI * getURI(Media *m);
    virtual ULONG getProvider();
    virtual void setOwning(bool owning);
    //serialize functions
    //get the #of bytes needed to serialize
    virtual int getSerializedLenImpl();
    //serialize
    //advance buffer, check if >= end
    //return 0 if OK
    virtual int serializeImpl(SerializeBuffer *b);
    //deserialize
    //advance buffer, check if >= end
    //return 0 if OK
    virtual int deserializeImpl(SerializeBuffer *b);
 };

#endif
