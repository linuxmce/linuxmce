//
// C++ Interface: Game_PlugIn
//
// Description:
//
//
// Author: Thomas Cherryhomes <thom.cherryhomes@localeconcept.com>
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCEGAMEMEDIASTREAM_H
#define DCEGAMEMEDIASTREAM_H

#include "../Media_Plugin/MediaHandlerInfo.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaDevice.h"

#include <map>
#include <list>
#include <string>

namespace DCE
{
	using namespace std;

	class GameMediaStream : public MediaStream
	{
		private:
			bool					m_bIsStreaming;

		public:
			int 	m_iPK_DesignObj_Remote_After_Menu; 		/** Store the real value if we're switching to a menu */
			int 	m_iPK_DesignObj_RemoteOSD_After_Menu;
	        int 	m_iPK_DesignObj_Remote_Popup_After_Menu;
		string m_sKeypadOverlayPath; /** Path to a keypad overlay, if used. */

			GameMediaStream(class Game_PlugIn *pGamePlugIn, class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider,MediaDevice *pMediaDevice, int PK_Users,enum SourceType sourceType,int iStreamID);

			virtual ~GameMediaStream();
			virtual int GetType();

			bool ShouldUseStreaming();

			bool ContainsVideo();

			bool CanPlayMore();
	};

};

#endif
