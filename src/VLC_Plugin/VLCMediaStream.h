/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef DCEVLCMEDIASTREAM_H
#define DCEVLCMEDIASTREAM_H

#include "../Media_Plugin/MediaHandlerInfo.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaDevice.h"

#include <map>
#include <list>
#include <string>

namespace DCE
{
  using namespace std;
  
  class VLCMediaStream : public MediaStream
  {
  private:
    bool	m_bIsStreaming;
    
  public:
    int 	m_iPK_DesignObj_Remote_After_Menu; 		/** Store the real value if we're switching to a menu */
    int 	m_iPK_DesignObj_RemoteOSD_After_Menu;
    int 	m_iPK_DesignObj_Remote_Popup_After_Menu;
    
    VLCMediaStream(class VLC_Plugin *pVLCPlugin, class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);
    
    virtual ~VLCMediaStream();
    virtual int GetType();
    
    bool ShouldUseStreaming();
    
    bool CanPlayMore();
  };  
};

#endif /** DCEVLCMEDIASTREAM_H */
