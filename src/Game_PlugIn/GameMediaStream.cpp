//
// C++ Implementation: Game_PlugIn
//
// Description:
//
//
// Author: Thomas Cherryhomes <thom.cherryhomes@localeconcept.com>
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "GameMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"

#include "pluto_main/Define_MediaType.h"

namespace DCE {

	GameMediaStream::GameMediaStream(
							class Game_PlugIn *pGamePlugIn,
							class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
							MediaDevice *pMediaDevice, 
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
		// m_bPlugInWillSetDescription = true;  // No longer needed.
		m_sKeypadOverlayPath="";
	}

	GameMediaStream::~GameMediaStream()
	{
	}

	int GameMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_GAME;
	}

	bool GameMediaStream::ShouldUseStreaming()
	{
		// if we have more than one target device.
		if ( m_mapEntertainArea.size() > 1 )
			return true;

		if ( m_mapEntertainArea.size() == 0 )
			return false;

		EntertainArea *pEntertainArea = m_mapEntertainArea.begin()->second;

		return false;
	}

	bool GameMediaStream::CanPlayMore()
	{

		return MediaStream::CanPlayMore();
	}

	bool GameMediaStream::ContainsVideo()
	{
		return true;	// need this to stop Pluto_Screen_Saver!
	}

};

