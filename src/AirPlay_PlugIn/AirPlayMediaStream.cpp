//
// C++ Implementation: AirPlay_PlugIn
//
// Description:
//
//
// Author: Serge Wagener <serge@swa.lu>
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "AirPlayMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {

	AirPlayMediaStream::AirPlayMediaStream(
							class AirPlay_PlugIn *pAirPlayPlugIn,
							class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
							MediaDevice *pMediaDevice, 
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
		m_bPlugInWillSetDescription = true; 
	}

	AirPlayMediaStream::~AirPlayMediaStream()
	{
	}

	int AirPlayMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_AIRPLAY_AUDIO;
	}

	bool AirPlayMediaStream::ShouldUseStreaming()
	{
		// if we have more than one target device.
		if ( m_mapEntertainArea.size() > 1 )
			return true;

		if ( m_mapEntertainArea.size() == 0 )
			return false;

		EntertainArea *pEntertainArea = m_mapEntertainArea.begin()->second;

		return false;
	}

	bool AirPlayMediaStream::CanPlayMore()
	{

		return MediaStream::CanPlayMore();
	}

	bool AirPlayMediaStream::ContainsVideo()
	{
		return false;	// sound only, no need to stop Pluto_Screen_Saver!
	}

};

