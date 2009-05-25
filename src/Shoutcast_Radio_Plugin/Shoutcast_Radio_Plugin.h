/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Shoutcast_Radio_Plugin_h
#define Shoutcast_Radio_Plugin_h

//	DCE Implemenation for #1929 Shoutcast Radio Plug-in

#include "Gen_Devices/Shoutcast_Radio_PluginBase.h"
//<-dceag-d-e->

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"
#include "../DCE/AlarmManager.h"
#include <vector>
#include <string>
#include <map>
#include <libxml/xmlreader.h>
//<-dceag-decl-b->
namespace DCE
{

        struct Station {
	  string name;
	  string mt; //mime type audio/mpeg or audio/aacp
	  int id;
	  int br;
	  string genre;
	  string ct; // current track
	  int lc;     // listener count
	};

	class ShoutcastUpdate {
	public:
	  char type; // G = genres, S = stations
	  uint genre; // genre if Station update
	};

	class Shoutcast_Radio_Plugin : public Shoutcast_Radio_Plugin_Command, public MediaHandlerBase, public AlarmEvent
	{
//<-dceag-decl-e->
		// Private member variables
private:
	string m_sBaseUrl;
	string m_sXMLUrl;
	string m_sTuneInBase;
	vector<string> m_vectGenres;
	map<int, Station> m_mapIDStation;
	map<string, vector<int> > m_mapGenreID;

	vector<string> m_vectGenresToLoad;
	int m_iLimit;
	int m_iGetInterval;
	int m_iRunInterval;

	AlarmManager* m_pAlarmManager;
	ShoutcastUpdate* m_pUpdate;

		// Private methods
	string getStationURL(int id);
	void AlarmCallback(int id, void* param);
	string getLimitURL();
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Shoutcast_Radio_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Shoutcast_Radio_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Shoutcast_Radio_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->
		
		/**
		* @brief
		*/
		virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

		/**
		* @brief Start media playback
		*/
		virtual bool StartMedia( class MediaStream *pMediaStream,string &sError );

		/**
		* @brief Stop media playback
		*/
		virtual bool StopMedia( class MediaStream *pMediaStream );

		virtual void PopulateDataGrid(string sToken,MediaListGrid *pMediaListGrid,int PK_MediaType, string sPK_Attribute, int PK_AttributeType_Sort, bool bShowFiles, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int iLastViewed, int *iPK_Variable, string *sValue_To_Assign );
		virtual void GetExtendedAttributes(string sType, string sPK_MediaSource, string sURL, string *sValue_To_Assign);


	vector<string> getGenres();
	void updateGenres();
	bool getStationsByGenre(string genre, vector<Station> &vectStations);
	void getStationById(vector<int> vectID, vector<Station> &vectStations);
	bool parseStation(xmlTextReaderPtr reader,Station& station);
	void updateStationsForGenre(string genre);
//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();
	string DATA_Get_Media_Catalog();

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
