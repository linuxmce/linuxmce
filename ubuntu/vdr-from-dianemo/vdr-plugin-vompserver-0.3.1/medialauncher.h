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

#ifndef MEDIALAUNCHER_H
#define MEDIALAUNCHER_H

#include "media.h"
#include "config.h"


class MediaLauncher
{
  public:
    MediaLauncher(Config *c);
    ~MediaLauncher();
    //let the launcher read it's config
    //return != 0 if nothing handled
    int init();
    //init as a copy of another launcher
    int init(MediaLauncher *cp);
    //get the type for a file (if the launcher can handle it)
    //return MEDIA_TYPE_UNKNOWN if it cannot be handled
    ULONG getTypeForName(const char *name);
    //open a stream, return PID of child , <0 on error
    int openStream(const char *filename,ULONG xsize, ULONG ysize,const char * command=NULL);
    //get the next chunk
    //if *buffer!= NULL - use this one, else allocate an own (with malloc!)
    //return read len (can be 0!)
    //return <0 on error, >0 on stream end,0 on OK
    int getNextBlock(ULONG size,unsigned char **buffer,ULONG *readLen);
    int closeStream();
    bool isOpen();

  private:
    Config *cfg;
    class MCommand{
      public:
      char *command;
      ULONG mediaType;
      char *extension;
      MCommand(const char *name,ULONG type,const char *ext);
      ~MCommand();
    } ;
    typedef MCommand *Pcmd;
    Pcmd *commands;
    int numcommands;
    int pnum;
    pid_t child;
    int findCommand(const char *name); //return -1 if not found


};

#endif
