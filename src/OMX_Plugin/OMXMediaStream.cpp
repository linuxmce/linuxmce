//
// C++ Implementation: OMX_Plugin
//
// Description:
//
//
// Author: Jason Murduck <phenigma@hotmail.com>
// Mostly copied from Xine_Plugin/XineMediaStream.cpp
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "OMXMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {
	OMXMediaStream::OMXMediaStream(
						class OMX_Plugin *pOMXPlugin,
						class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
						MediaDevice *pMediaDevice,
						int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users, sourceType, iStreamID)
	{
		m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
	}

	OMXMediaStream::~OMXMediaStream()
	{
	}

	int OMXMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_OMX;
	}

	bool OMXMediaStream::ShouldUseStreaming()
	{
		// if we have more than one target device.
		if ( m_mapEntertainArea.size() > 1 )
			return true;

		if ( m_mapEntertainArea.size() == 0 )
			return false;

		EntertainArea *pEntertainArea = m_mapEntertainArea.begin()->second;

		return false;
	}

	bool OMXMediaStream::CanPlayMore()
	{
		// do not remove the playlist when we are playing stored audio. (it will just confuse the user)
		if ( m_iPK_MediaType == MEDIATYPE_pluto_StoredAudio_CONST && m_iRepeat != -1)
			return true;

		return MediaStream::CanPlayMore();
	}

//	bool OMXMediaStream::ContainsVideo()
//	{
//		return true;	// need this to stop Photo_Screen_Saver
//	}
};
