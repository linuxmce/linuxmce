//
// C++ Implementation: SmartQ_Media_Plugin
//
// Description:
//
//
// Author: Thomas Cherryhomes <thom.cherryhomes@localeconcept.com>
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "SmartQMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {

	SmartQMediaStream::SmartQMediaStream(
							class SmartQ_Media_Plugin *pSmartQPlugIn,
							class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
							MediaDevice *pMediaDevice, 
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
		m_bPlugInWillSetDescription = true; 
	}

	SmartQMediaStream::~SmartQMediaStream()
	{
	}

	int SmartQMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_SMARTQ;
	}

	bool SmartQMediaStream::ShouldUseStreaming()
	{
		// if we have more than one target device.
		if ( m_mapEntertainArea.size() > 1 )
			return true;

		if ( m_mapEntertainArea.size() == 0 )
			return false;

		EntertainArea *pEntertainArea = m_mapEntertainArea.begin()->second;

		return false;
	}

	bool SmartQMediaStream::CanPlayMore()
	{

		return MediaStream::CanPlayMore();
	}

	bool SmartQMediaStream::ContainsVideo()
	{
		return true;	// need this to stop Pluto_Screen_Saver!
	}

};

