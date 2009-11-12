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

#include "mediaplayer.h"
#include "media.h"
#include "log.h"

class MediaProviderHolder {
  public:
    MediaProvider *provider;
    ULONG         id;
    ULONG         range;
    MediaProviderHolder(MediaProvider *p,ULONG i, ULONG r=1) {
      provider=p;
      range=r;
      id=i;
    }
};



MediaPlayer::MediaPlayer(){}
MediaPlayer::~MediaPlayer(){
  for (Tplist::iterator it=plist.begin();it<plist.end();it++) {
  delete *it;
  }
}

/**
  * get the root media list
  * the returned list has to be destroyed by the caller
  * if NULL is returned currently no media is available
  */
MediaList* MediaPlayer::getRootList(){
  Log::getInstance()->log("MediaPlayer::getRootList",Log::DEBUG,"numproviders %d",plist.size());
  MediaList * rt=new MediaList(new MediaURI(0,NULL,NULL));
  for (Tplist::iterator it=plist.begin();it<plist.end();it++) {
    MediaList *cur=(*it)->provider->getRootList();
    if (cur) {
      //we take the entries away from the list - so don't delete them with the list
      cur->setOwning(false);
      Log::getInstance()->log("MediaPlayer::getRootList",Log::DEBUG,"got list for provider %p with %d items",(*it),cur->size());
      for (MediaList::iterator mi=cur->begin();mi<cur->end();mi++) {
        Media *m=*mi;
        //always set the URI in the root list because we combine entries
        //from different lists
        if (! m->getURI()) {
          MediaURI *u=cur->getURI(m);
          m->setURI(u);
          Log::getInstance()->log("MediaPlayer::getRootList",Log::DEBUG,"set uri n=%s,d=%s",u->getName(),u->getDisplayName());
          delete u;
        }
        rt->push_back(m);
        Log::getInstance()->log("MediaPlayer::getRootList",Log::DEBUG,"added item to list name=%s",m->getFileName());
      }
    }
    delete cur;
  }
  return rt;
}

/**
  * get a medialist for a given parent
  * the returned list has to be destroyed by the caller
  * NULL if no entries found
  */
MediaList* MediaPlayer::getMediaList(const MediaURI * parent){
  Log::getInstance()->log("MediaPlayer::getMediaList",Log::DEBUG,"numproviders %d,parent=%p",plist.size(),parent);
  MediaProvider *p=providerById(parent->getProvider());
  if (! p) {
    return NULL;
  }
  return p->getMediaList(parent);
}

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
int MediaPlayer::openMedium(ULONG channel, const MediaURI * uri, ULLONG * size, ULONG xsize, ULONG ysize){
  if ( channel >= NUMCHANNELS) return -1;
  info[channel].provider=NULL;
  *size= 0;
  MediaProvider *p=providerById(uri->getProvider());
  if (!p) {
    return -1;
  }
  int rt=p->openMedium(channel,uri,size,xsize,ysize);
  if (rt == 0) {
    info[channel].providerId=uri->getProvider();
    info[channel].provider=p;
  }
  return rt;

}

/**
  * get a block for a channel
  * @param offset - the offset
  * @param len - the required len
  * @param outlen out - the read len if 0 this is EOF
  * @param buffer out the allocated buffer (must be freed with free!)
  * @return != 0 in case of error
  */           
int MediaPlayer::getMediaBlock(ULONG channel, ULLONG offset, ULONG len, ULONG * outlen,
    unsigned char ** buffer) {
  if ( channel >= NUMCHANNELS) return -1;
  if ( info[channel].provider == NULL) return -1;
  return info[channel].provider->getMediaBlock(channel,offset,len,outlen,buffer);
}


/**
  * close a media channel
  */
int MediaPlayer::closeMediaChannel(ULONG channel){
  if ( channel >= NUMCHANNELS) return -1;
  if ( info[channel].provider == NULL) return -1;
  int rt=info[channel].provider->closeMediaChannel(channel);
  info[channel].provider=NULL;
  info[channel].providerId=0;
  return rt;
}

/**
  * return the media info for a given channel
  * return != 0 on error
  * the caller has to provide a pointer to an existing media info
  */
int MediaPlayer::getMediaInfo(ULONG channel, struct MediaInfo * result){
  if ( channel >= NUMCHANNELS) return -1;
  if ( info[channel].provider == NULL) return -1;
  return info[channel].provider->getMediaInfo(channel,result);
}



void MediaPlayer::registerMediaProvider(MediaProvider *p,ULONG providerId,ULONG range) {
  if (! p) return;
  MediaProviderHolder *h=new MediaProviderHolder(p,providerId,range);
  Log::getInstance()->log("MediaPlayer::registerMediaProvider",Log::DEBUG,"p=%p",p);
  plist.push_back(h);
}

MediaProvider * MediaPlayer::providerById(ULONG id) {
  MediaProvider *rt=NULL;
  for (Tplist::iterator it=plist.begin();it<plist.end();it++) {
    MediaProviderHolder *h=*it;
    if (id >= h->id && id < (h->id+h->range)) {
      rt=h->provider;
      break;
    }
  }
  Log::getInstance()->log("MediaPlayer::providerById",Log::DEBUG,"id=%d,p=%p",id,rt);
  return rt;
}

MediaPlayer* MediaPlayer::getInstance() {
  return (MediaPlayer *) MediaPlayerRegister::getInstance();
}

MediaPlayerRegister* MediaPlayerRegister::getInstance() {
  if ( ! instance) {
    instance=new MediaPlayer();
  }
  return instance;
}
MediaPlayerRegister *MediaPlayerRegister::instance=NULL;





