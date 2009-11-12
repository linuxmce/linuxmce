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

#ifndef SERVERMEDIAFILE_H
#define SERVERMEDIAFILE_H

#include "mediafile.h"
#include "config.h"

class MediaLauncher;


class ServerMediaFile : public MediaFile 
{
  public:
    ServerMediaFile(Config *c,MediaPlayerRegister *distributor);
    virtual ~ServerMediaFile();
    virtual MediaList* getRootList();
    virtual int openMedium(ULONG channel, const MediaURI * uri, ULLONG * size, ULONG xsize, ULONG ysize);
    virtual int getMediaBlock(ULONG channel, ULLONG offset, ULONG len, ULONG * outlen,
        unsigned char ** buffer);
    virtual int closeMediaChannel(ULONG channel);
    virtual int getMediaInfo(ULONG channel, MediaInfo * result);
    virtual MediaList* getMediaList(const MediaURI *parent);



  protected:
    virtual ULONG getMediaType(const char *fname);
    MediaLauncher * launchers[NUMCHANNELS];
    MediaLauncher * dirhandler;

  private:
    Config *cfg;
    ULONG addDataToList(unsigned char * buf, ULONG buflen,MediaList *list,bool extendedFormat) ;


};

#endif
