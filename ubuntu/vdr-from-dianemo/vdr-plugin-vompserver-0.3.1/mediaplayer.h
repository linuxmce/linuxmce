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

#ifndef MEDIAPLAYER
#define MEDIAPLAYER

using namespace std;
#include <vector>
#include <stdio.h>
#include <string.h>
#include "mediaprovider.h"

class MediaProviderHolder;


class MediaPlayer : public MediaPlayerRegister, public MediaProvider
{
  public:
    MediaPlayer();
    virtual ~MediaPlayer();

    /**
      * get the root media list
      * the returned list has to be destroyed by the caller
      * if NULL is returned currently no media is available
      */
    virtual MediaList* getRootList();

    /**
      * get a medialist for a given parent
      * the returned list has to be destroyed by the caller
      * NULL if no entries found
      */
    virtual MediaList* getMediaList(const MediaURI * parent);

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
    virtual int openMedium(ULONG channel, const MediaURI * uri, ULLONG * size, ULONG xsize, ULONG ysize);

    /**
      * get a block for a channel
      * @param offset - the offset
      * @param len - the required len
      * @param outlen out - the read len if 0 this is EOF
      * @param buffer out the allocated buffer (must be freed with free!)
      * @return != 0 in case of error
      */           
    virtual int getMediaBlock(ULONG channel, ULLONG offset, ULONG len, ULONG * outlen,
        unsigned char ** buffer);

    /**
      * close a media channel
      */
    virtual int closeMediaChannel(ULONG channel);

    /**
      * return the media info for a given channel
      * return != 0 on error
      * the caller has to provide a pointer to an existing media info
      */
    virtual int getMediaInfo(ULONG channel, struct MediaInfo * result);
    
    /**
      * from MediaPlayerRegister
      */
    virtual void registerMediaProvider(MediaProvider *p,ULONG providerID,ULONG range);

    /**
      * the instance
      */
    static MediaPlayer * getInstance();

  private:
    MediaProvider * providerById(ULONG id);
    typedef vector<MediaProviderHolder *> Tplist;
    Tplist plist;
    struct channelInfo {
      ULONG providerId;
      MediaProvider *provider;
      channelInfo() {
        provider=NULL;
        providerId=0;
      }
      };
    struct channelInfo info[NUMCHANNELS];

};


#endif
