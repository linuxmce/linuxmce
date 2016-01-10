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
#ifndef MediaHandlerBase_h
#define MediaHandlerBase_h

#include "EntertainArea.h"

#include "MediaListGrid.h"

/**
* A Media Handler is derived from the Media Handler abstract class.  When it registers, it passes in a MediaHandlerInfo pointer indicating
* what type of media it can play.  It may register several times with different types of media and different capabilities.
*/

namespace DCE
{

	/** All media plugins must implement this class */
	class MediaHandlerBase
	{
	public:
		class Media_Plugin *m_pMedia_Plugin;
		bool m_bPreProcessSpeedControl;  // If true, media plugin will do some processing, like converting 2 pauses to a play, and repeated 'ff' to increasing change playback speed

		/** @brief constructor */
		MediaHandlerBase() : m_pMedia_Plugin(NULL), m_bPreProcessSpeedControl(true) { }
		virtual ~MediaHandlerBase() { }

		int m_iPriority; // If multiple media handlers can handle the same type of media in an area the one with the highest priority will be chosen
		vector<class MediaHandlerInfo *> m_vectMediaHandlerInfo; // The media handler info's we have

		/** @brief Each Plugin will create its own instance of MediaStream, so it can create a derived version with extra information */
		virtual class MediaStream *CreateMediaStream(class MediaHandlerInfo *pMediaHandlerInfo,int iPK_MediaProvider,vector<class EntertainArea *> &vectEntertainArea,MediaDevice *pMediaDevice,int iPK_Users, deque<MediaFile *> *dequeMediaFile,int StreamID)=0;
		/** If a plugin cannot start the media, it can supply an error message explaining why */
		virtual bool StartMedia(class MediaStream *pMediaStream,string &sError);
		virtual bool StopMedia(class MediaStream *pMediaStream);

		// Given a stream, what is the rendering device(s).  The source device is stored in the stream.  Normally the source and rendering are the same (dvd player, for example).
		// But sometimes the source may be a back-end streamer, and the rendering device(s) some network audio players.  The framework needs
		// to know the rendering devices so it can send on/off's to them.  If the source & dest aren't the same, the plug-in must override this
		virtual void GetRenderDevices(EntertainArea *pEntertainArea,map<int,MediaDevice *> *pmapMediaDevice);
		void GetRenderDevices(map<int, MediaDevice *> *pmapMediaDevices);  // In all entertainment areas

		// it can't virtual and static at the same time.
		virtual MediaDevice *GetMediaDeviceForEntertainArea(EntertainArea *pEntertainArea, int PK_DeviceTemplate)
		{
			list<MediaDevice*> *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(PK_DeviceTemplate);
			if( pListMediaDevice && pListMediaDevice->size())
				return pListMediaDevice->front();
			else
				return NULL;
		}

		virtual void PopulateDataGrid(string sToken,MediaListGrid *pMediaListGrid,int PK_MediaType, string sPK_Attribute, int PK_AttributeType_Sort, bool bShowFiles, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int iLastViewed, int *iPK_Variable, string *sValue_To_Assign ) {}
		virtual void GetExtendedAttributes(string sType, string sPK_MediaSource, string sURL, string *sValue_To_Assign) {};
	};

}

#endif
