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
#ifndef EntertainArea_h
#define EntertainArea_h

#include "MediaDevice.h"
#include "MediaHandlerInfo.h"
#include "BoundRemote.h"
#include "MediaStream.h"

namespace DCE
{
	class Room;

	/** @brief An entertainment area is a cluster of devices.  It's analogous to a Room, except you can have multiple entertainment areas in a room, like
	* a 'his' and 'hers' tv in the bedroom.
	*/

	class EntertainArea
	{
	public:

		/** @brief constructor*/

		EntertainArea(int iPK_EntertainArea,bool bOnly1Stream,string sDescription,Room *pRoom)
		{ m_iPK_EntertainArea=iPK_EntertainArea; m_bOnly1Stream=bOnly1Stream; m_pMediaStream=NULL; m_sDescription=sDescription; m_pRoom=pRoom; m_pMediaDevice_ActiveDest=NULL; m_bMediaIsPrivate=false; m_pOH_Orbiter_OSD=NULL; m_pMediaDevice_MD=NULL; m_pMediaDevice_OutputZone=NULL; m_bViewingLiveAVPath=false; }


		~EntertainArea()
		{
			for(map<int,ListMediaDevice *>::iterator it=m_mapMediaDeviceByTemplate.begin();it!=m_mapMediaDeviceByTemplate.end();++it)
				delete (*it).second;
			for(map< pair<int,int>,List_MediaHandlerInfo *>::iterator it=m_mapMediaHandlerInfo_MediaType.begin();it!=m_mapMediaHandlerInfo_MediaType.end();++it)
				delete (*it).second;
			for(map<string,List_MediaHandlerInfo *>::iterator it=m_mapMediaHandlerInfo_Extension.begin();it!=m_mapMediaHandlerInfo_Extension.end();++it)
				delete (*it).second;
			for(MapBoundRemote::iterator it=m_mapBoundRemote.begin();it!=m_mapBoundRemote.end();++it)
				delete (*it).second;

			delete m_pMediaStream;
			m_pMediaStream = NULL;
		}

		int m_iPK_EntertainArea;
		string m_sDescription;
		Room *m_pRoom;
		bool m_bMediaIsPrivate;
		
		class MediaStream  *m_pMediaStream;   /** The current media streams in this entertainment area */
		MediaDevice	*m_pMediaDevice_ActiveDest;      /** The media device that is currently rendering media in this area */
		MediaDevice	*m_pMediaDevice_OutputZone;      /** The media device that is used as a special output zone */
		MediaDevice	*m_pMediaDevice_MD;      /** The media device that is the MD in this area */
		class OH_Orbiter *m_pOH_Orbiter_OSD;    	   /** Which orbiter is the on-screen display for this entertainment area */

		map<int, class MediaDevice *> m_mapMediaDevice;  /** All the media devices in the area */
		MediaDevice *m_mapMediaDevice_Find(int PK_Device) { map<int,class MediaDevice *>::iterator it = m_mapMediaDevice.find(PK_Device); return it==m_mapMediaDevice.end() ? NULL : (*it).second; }
		map<int, ListMediaDevice *> m_mapMediaDeviceByTemplate;  /** All the media devices in the area by device template */
		ListMediaDevice *m_mapMediaDeviceByTemplate_Find(int PK_DeviceTemplate) { map<int,ListMediaDevice *>::iterator it = m_mapMediaDeviceByTemplate.find(PK_DeviceTemplate); return it==m_mapMediaDeviceByTemplate.end() ? NULL : (*it).second; }
		ListMediaDevice m_listVFD_LCD_Displays;
		bool m_bViewingLiveAVPath; // True if the device has video that runs both through the capture card, and directly to other av devices, and the user is viewing it directly to the a/v devices.  Sending the Live_AVPath command makes this happen

		/** If the user starts some media temporarily (like an announcement) and wants the prior
		media to continue when it's done, the prior interrupted media streams are stored here */
		vector<class MediaStream  *> m_vectMediaStream_Interrupted;

		// These are the Orbiters that are currently controling this entertainment area.  When an
		// orbiter goes to a remote control for an entertainment area it becomes "bound".
		MapBoundRemote m_mapBoundRemote;

		BoundRemote *m_mapBoundRemote_Find(int iPK_Orbiter)
		{
			map<int,class BoundRemote *>::iterator it = m_mapBoundRemote.find(iPK_Orbiter);
			return it==m_mapBoundRemote.end() ? NULL : (*it).second;
		}

		void m_mapBoundRemote_Remove(int iPK_Orbiter)
		{
			MapBoundRemote::iterator it = m_mapBoundRemote.find(iPK_Orbiter);
			if( it!=m_mapBoundRemote.end() ) 
			{
				delete it->second;
				m_mapBoundRemote.erase(it);
			}
		}

		bool m_bOnly1Stream;
		/** If true, stop any other media streams in this area when a new one starts.  There can be only 1.  This is the default behavior */

		map< pair<int,int>,List_MediaHandlerInfo *> m_mapMediaHandlerInfo_MediaType; /** The key is the media type,media provider */

		List_MediaHandlerInfo *m_mapMediaHandlerInfo_MediaType_Find(int PK_MediaType,int PK_MediaProvider)
		{
			map< pair<int,int>,List_MediaHandlerInfo *>::iterator it = m_mapMediaHandlerInfo_MediaType.find( make_pair (PK_MediaType,PK_MediaProvider) );
			return it==m_mapMediaHandlerInfo_MediaType.end() ? NULL : (*it).second;
		}

		map<string,List_MediaHandlerInfo *> m_mapMediaHandlerInfo_Extension; // The key is a file extension
		List_MediaHandlerInfo *m_mapMediaHandlerInfo_Extension_Find(string Extension) {
			map<string,List_MediaHandlerInfo *>::iterator it = m_mapMediaHandlerInfo_Extension.find(Extension);
			return it==m_mapMediaHandlerInfo_Extension.end() ? NULL : (*it).second;
		}
	};

	typedef map<int,class EntertainArea *> MapEntertainArea;
}

#endif
