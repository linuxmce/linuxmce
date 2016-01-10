/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include "VLCMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {
  
  VLCMediaStream::VLCMediaStream(
				 class VLC_Plugin *pVLCPlugin,
				 class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
				 MediaDevice *pMediaDevice, 
				 int PK_Users,enum SourceType sourceType,int iStreamID)
    : MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
  {
    m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
  }
  
  VLCMediaStream::~VLCMediaStream()
  {
  }
  
  int VLCMediaStream::GetType()
  {
    return MEDIASTREAM_TYPE_VIDEOLAN;
  }
  
  bool VLCMediaStream::ShouldUseStreaming()
  {
    // if we have more than one target device.
    if ( m_mapEntertainArea.size() > 1 )
      return true;
    
    if ( m_mapEntertainArea.size() == 0 )
      return false;
    
    EntertainArea *pEntertainArea = m_mapEntertainArea.begin()->second;
    if ( pEntertainArea->m_pMediaDevice_ActiveDest && pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_SqueezeBox_Player_CONST)
      return true;
    
    return false;
  }
  
  bool VLCMediaStream::CanPlayMore()
  {
    // do not remove the playlist when we are playing stored audio. (it will just confuse the user)
    if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat != -1)
      return true;
    
    return MediaStream::CanPlayMore();
  }
};
