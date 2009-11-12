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

#ifndef VDRCOMMAND_H
#define VDRCOMMAND_H

#include "defines.h"
#include "serialize.h"
#include "media.h"

/**
  * data holder for VDR commands
  * it's only important to add serializable objects
  * in the same order on both sides
  */

//until we really have response - commands we simply take
//the request+this flag for responses
//not really necessary but for checks it's better to have a command ID at least in some responses
const static ULONG VDR_RESPONSE_FLAG =0x1000000;

//as this header is only included by vdr.cc the constants are this way private
//but can easily be used on the server side as well

const static ULONG VDR_LOGIN               = 1;
const static ULONG VDR_GETRECORDINGLIST    = 2;
const static ULONG VDR_DELETERECORDING     = 3;
const static ULONG VDR_GETCHANNELLIST      = 5;
const static ULONG VDR_STREAMCHANNEL       = 6;
const static ULONG VDR_GETBLOCK            = 7;
const static ULONG VDR_STOPSTREAMING       = 8;
const static ULONG VDR_STREAMRECORDING     = 9;
const static ULONG VDR_GETCHANNELSCHEDULE  = 10;
const static ULONG VDR_CONFIGSAVE          = 11;
const static ULONG VDR_CONFIGLOAD          = 12;
const static ULONG VDR_RESCANRECORDING     = 13;  // FIXME obselete
const static ULONG VDR_GETTIMERS           = 14;
const static ULONG VDR_SETTIMER            = 15;
const static ULONG VDR_POSFROMFRAME        = 16;
const static ULONG VDR_FRAMEFROMPOS        = 17;
const static ULONG VDR_MOVERECORDING       = 18;
const static ULONG VDR_GETNEXTIFRAME       = 19;
const static ULONG VDR_GETRECINFO          = 20;
const static ULONG VDR_GETMARKS            = 21;
const static ULONG VDR_GETCHANNELPIDS      = 22;
const static ULONG VDR_DELETETIMER         = 23;
const static ULONG VDR_GETLANGUAGELIST     = 33;
const static ULONG VDR_GETLANGUAGECONTENT  = 34;
const static ULONG VDR_GETMEDIALIST        = 30;
const static ULONG VDR_OPENMEDIA           = 31;
const static ULONG VDR_GETMEDIABLOCK       = 32;
const static ULONG VDR_GETMEDIAINFO        = 35;
const static ULONG VDR_CLOSECHANNEL        = 36;

class VDR_Command : public SerializableList {
  public:
    VDR_Command(const ULONG cmd) {
      command=cmd;
      addParam(&command);
    }
    virtual ~VDR_Command(){}
    ULONG command;
};

class VDR_GetMediaListRequest : public VDR_Command {
  public:
    VDR_GetMediaListRequest(MediaURI *root) :VDR_Command(VDR_GETMEDIALIST) {
      addParam(root);
    }
};

class VDR_GetMediaListResponse : public VDR_Command {
  public:
    VDR_GetMediaListResponse(ULONG *flags,MediaList *m) : VDR_Command(VDR_GETMEDIALIST|VDR_RESPONSE_FLAG){
      addParam(flags);
      addParam(m);
    }
};

class VDR_OpenMediumRequest : public VDR_Command {
  public:
    VDR_OpenMediumRequest(ULONG *channel,MediaURI *u,ULONG *xsize, ULONG *ysize) :
      VDR_Command(VDR_OPENMEDIA) {
        addParam(channel);
        addParam(u);
        addParam(xsize);
        addParam(ysize);
      }
};
class VDR_OpenMediumResponse : public VDR_Command {
  public:
    VDR_OpenMediumResponse(ULONG *flags,ULLONG *size) :
      VDR_Command(VDR_OPENMEDIA|VDR_RESPONSE_FLAG) {
        addParam(flags);
        addParam(size);
      }
};
class VDR_GetMediaBlockRequest : public VDR_Command {
  public:
    VDR_GetMediaBlockRequest(ULONG * channel, ULLONG *pos, ULONG *max):
      VDR_Command(VDR_GETMEDIABLOCK) {
        addParam(channel);
        addParam(pos);
        addParam(max);
      }
};

//no response class for GetMediaBlock


class VDR_CloseMediaChannelRequest : public VDR_Command {
  public:
    VDR_CloseMediaChannelRequest(ULONG * channel):
      VDR_Command(VDR_CLOSECHANNEL) {
        addParam(channel);
      }
};

class VDR_CloseMediaChannelResponse : public VDR_Command {
  public:
    VDR_CloseMediaChannelResponse(ULONG * flags):
      VDR_Command(VDR_CLOSECHANNEL|VDR_RESPONSE_FLAG) {
        addParam(flags);
      }
};

class VDR_GetMediaInfoRequest : public VDR_Command {
  public:
    VDR_GetMediaInfoRequest(ULONG * channel):
      VDR_Command(VDR_GETMEDIAINFO) {
        addParam(channel);
      }
};
class VDR_GetMediaInfoResponse : public VDR_Command {
  public:
    VDR_GetMediaInfoResponse(ULONG * flags,MediaInfo *info):
      VDR_Command(VDR_GETMEDIAINFO|VDR_RESPONSE_FLAG) {
        addParam(flags);
        addParam(info);
      }
};




#endif
