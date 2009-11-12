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

#ifndef MEDIAPROVIDER_H
#define MEDIAPROVIDER_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "defines.h"

class Media;
class MediaURI;
class MediaInfo;
class MediaList;


/**
  this interface has to be implemented by 
  any provider of media data.
  In all URIs the provider has to insert providerIds out of its range.
  threading issues:
  all operations to one channel are not thread save - so users have to ensure
  that at most one thread at a time is accesing operations to one channel.
  Implementers have to ensure that other operations are thread safe.
  Exception: registering providers is not thread safe (at least at the moment).
  **/

//the max number of media channels used in parallel
#define NUMCHANNELS 3
//name of a media file
#define NAMESIZE 255 

class MediaProvider
{
  public:
    MediaProvider(){}
    virtual ~MediaProvider(){}

    /**
      * get the root media list
      * the returned list has to be destroyed by the caller
      * if NULL is returned currently no media is available
      */
    virtual MediaList* getRootList()=0;

    /**
      * get a medialist for a given parent
      * the returned list has to be destroyed by the caller
      * NULL if no entries found
      */
    virtual MediaList* getMediaList(const MediaURI * parent)=0;

    /**
      * open a media uri
      * afterwards getBlock or other functions must be possible
      * currently only one medium is open at the same time
      * for a given channel
      * @param channel: channel id, NUMCHANNELS must be supported
      * @param xsize,ysize: size of the screen
      * @param size out: the size of the medium
      * @return != 0 in case of error
      * 
      */
    virtual int openMedium(ULONG channel, const MediaURI * uri, ULLONG * size, ULONG xsize, ULONG ysize)=0;

    /**
      * get a block for a channel
      * @param offset - the offset
      * @param len - the required len
      * @param outlen out - the read len if 0 this is EOF
      * @param buffer out the allocated buffer (must be freed with free!)
      *        if buffer is set at input the implementation CAN use
      *        this buffer - it is up to the caller to test if the buffer
      *        is at the same value when the method returns.
      *        it is assumed that there is enough room in the buffer if it set
      *        when calling
      * @return != 0 in case of error
      */           
    virtual int getMediaBlock(ULONG channel, ULLONG offset, ULONG len, ULONG * outlen,
        unsigned char ** buffer)=0;

    /**
      * close a media channel
      */
    virtual int closeMediaChannel(ULONG channel)=0;

    /**
      * return the media info for a given channel
      * return != 0 on error
      * the caller has to provide a pointer to an existing media info
      */
    virtual int getMediaInfo(ULONG channel, MediaInfo * result)=0;

};


/**
  * the mediaplayer to register providers at
  * can be static ctor's
  */
class MediaPlayerRegister {
  public:
    virtual void registerMediaProvider(MediaProvider *pi,ULONG id,ULONG range=1)=0;
    virtual ~MediaPlayerRegister(){}
    static MediaPlayerRegister* getInstance();
  protected:
    static MediaPlayerRegister *instance;
};


#endif
