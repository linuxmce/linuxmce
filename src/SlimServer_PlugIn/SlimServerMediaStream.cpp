/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//
// C++ Implementation: SlimServer_PlugIn
//
// Description:
//
//
// Author: Toader Mihai Claudiu mihai.t@plutohome.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "SlimServerMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {

	SlimServerMediaStream::SlimServerMediaStream(
							class SlimServer_PlugIn *pSlimServerPlugin,
							class MediaHandlerInfo *pMediaHandlerInfo,
							int iPK_MediaProvider,
							MediaDevice *pMediaDevice,
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_bIsStreaming = 0;
	}

	SlimServerMediaStream::~SlimServerMediaStream()
	{
	}

	int SlimServerMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_SLIMSERVER;
	}

	bool SlimServerMediaStream::isStreaming()
	{
		return m_bIsStreaming;
	}

	void SlimServerMediaStream::setIsStreaming(bool isStreaming)
	{
		m_bIsStreaming = isStreaming;
	}

	bool SlimServerMediaStream::ShouldUseStreaming()
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

	bool SlimServerMediaStream::CanPlayMore()
	{
		// do not remove the playlist when we are playing stored audio. (it will just confuse the user)
		if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat != -1)
			return true;

		return MediaStream::CanPlayMore();
	}
};
