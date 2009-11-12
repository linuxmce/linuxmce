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

#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include "defines.h"
#include "mediaprovider.h"


class MediaFile : public MediaProvider 
{
  public:
    MediaFile(ULONG providerId);
    virtual ~MediaFile();
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
    virtual int getMediaInfo(ULONG channel, MediaInfo * result);


  protected:
    /**
      * create a Media out of a given file
      * the Media object ha sto be deleted by the caller
      * return NULL if not there or no matching type
      */
    Media * createMedia(const char * dirname, const char * filename, bool withURI=false);
    struct ChannelInfo {
      public:
      FILE *file;
      ULONG  provider;
      char *filename;
      ULLONG size;
      ChannelInfo(){
        file=NULL;
        provider=0;
        filename=NULL;
        size=0;
      }
      ~ChannelInfo(){
        if (filename) delete[]filename;
        if (file) fclose(file);
      }
      void setFilename(const char *f) {
        if (filename) delete[] filename;
        filename=NULL;
        if (f) {
          filename=new char[strlen(f)+1];
          strcpy(filename,f);
        }
      }
      void reset() {
        if (file) fclose(file);
        file=NULL;
        if (filename) delete [] filename;
        filename=NULL;
      }
    };
    struct ChannelInfo channels[NUMCHANNELS];
    ULONG providerid;

    virtual ULONG getMediaType(const char *filename);

};

#endif
