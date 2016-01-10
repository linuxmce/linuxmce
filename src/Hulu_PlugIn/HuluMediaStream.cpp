//
// C++ Implementation: Hulu_PlugIn
//
// Description:
//
//
// Author: Thomas Cherryhomes <thom.cherryhomes@localeconcept.com>
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "HuluMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {

	HuluMediaStream::HuluMediaStream(
							class Hulu_PlugIn *pHuluPlugIn,
							class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
							MediaDevice *pMediaDevice, 
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
		m_bPlugInWillSetDescription = true; 
	}

	HuluMediaStream::~HuluMediaStream()
	{
	}

	int HuluMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_HULU;
	}

	bool HuluMediaStream::ShouldUseStreaming()
	{
		// if we have more than one target device.
		if ( m_mapEntertainArea.size() > 1 )
			return true;

		if ( m_mapEntertainArea.size() == 0 )
			return false;

		EntertainArea *pEntertainArea = m_mapEntertainArea.begin()->second;

		return false;
	}

	bool HuluMediaStream::CanPlayMore()
	{

		return MediaStream::CanPlayMore();
	}

	bool HuluMediaStream::ContainsVideo()
	{
		return true;	// need this to stop Pluto_Screen_Saver!
	}

};

