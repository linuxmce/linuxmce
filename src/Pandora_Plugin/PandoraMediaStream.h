/**
 * PandoraMediaStream - MediaStream subclass for Pandora Web Radio Streams
 * 
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * 
 * @version 1.0
 * 
 */

#include "../Media_Plugin/MediaHandlerInfo.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaDevice.h"

#include <map>
#include <list>
#include <string>

#ifndef PANDORAMEDIASTREAM_H
#define PANDORAMEDIASTREAM_H

namespace DCE
{
	using namespace std;

	class PandoraMediaStream : public MediaStream
	{
		private:
			bool					m_bIsStreaming;

		public:
			int 	m_iPK_DesignObj_Remote_After_Menu; 		/** Store the real value if we're switching to a menu */
			int 	m_iPK_DesignObj_RemoteOSD_After_Menu;
	        int 	m_iPK_DesignObj_Remote_Popup_After_Menu;

			PandoraMediaStream(class Pandora_Plugin *pPandoraPlugin, class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);

			virtual ~PandoraMediaStream();
			virtual int GetType();

			bool ShouldUseStreaming();

			bool ContainsVideo();

			bool CanPlayMore();
	};

};

#endif /* PANDORAMEDIASTREAM_H */