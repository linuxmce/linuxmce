/**
 * PandoraMediaStream - MediaStream subclass for Pandora Web Radio Streams
 * 
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * 
 * @version 1.0
 * 
 */

#include "PandoraMediaStream.h"
#include "../Media_Plugin/EntertainArea.h"
#include "pluto_main/Define_MediaType.h"

namespace DCE {

	PandoraMediaStream::PandoraMediaStream(
							class Pandora_Plugin *pPandoraPlugin,
							class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,
							MediaDevice *pMediaDevice, 
							int PK_Users,enum SourceType sourceType,int iStreamID)
				: MediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, PK_Users,sourceType, iStreamID)
	{
		m_iPK_DesignObj_Remote_After_Menu=m_iPK_DesignObj_RemoteOSD_After_Menu=m_iPK_DesignObj_Remote_Popup_After_Menu=0;
		m_bPlugInWillSetDescription = true; 
	}

	PandoraMediaStream::~PandoraMediaStream()
	{
	}

	int PandoraMediaStream::GetType()
	{
		return MEDIASTREAM_TYPE_PANDORA;
	}

	bool PandoraMediaStream::ShouldUseStreaming()
	{
		// if we have more than one target device.
		if ( m_mapEntertainArea.size() > 1 )
			return true;

		if ( m_mapEntertainArea.size() == 0 )
			return false;

		EntertainArea *pEntertainArea = m_mapEntertainArea.begin()->second;

		return false;
	}

	bool PandoraMediaStream::CanPlayMore()
	{

		return MediaStream::CanPlayMore();
	}

	bool PandoraMediaStream::ContainsVideo()
	{
		return true;	// need this to stop Pluto_Screen_Saver!
	}

};

