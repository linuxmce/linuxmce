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
#include "Shoutcast_Radio_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/HttpUtils.h"
#include "PlutoUtils/StringUtils.h"
#include <libxml/xmlreader.h>

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#define UPDATE_SHOUTCAST 1

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Shoutcast_Radio_Plugin::Shoutcast_Radio_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Shoutcast_Radio_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
        m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(1);

	// We re-use this update description object troughout the life of the plugin
	m_pUpdate = new ShoutcastUpdate();
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Shoutcast_Radio_Plugin::Shoutcast_Radio_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Shoutcast_Radio_Plugin_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Shoutcast_Radio_Plugin::~Shoutcast_Radio_Plugin()
//<-dceag-dest-e->
{
	m_pAlarmManager->Stop();
        delete m_pAlarmManager;
	m_pAlarmManager = NULL;
        if (m_pUpdate != NULL) {
                delete m_pUpdate;
	}
}

//<-dceag-getconfig-b->
bool Shoutcast_Radio_Plugin::GetConfig()
{
	if( !Shoutcast_Radio_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	string sConfig = DATA_Get_Configuration();

	vector<string> vectString;
	StringUtils::Tokenize(sConfig,"\n",vectString);
	for(uint i=0; i < vectString.size(); ++i) {
	        string s = StringUtils::Replace(vectString[i],"\r","");
		if (StringUtils::StartsWith(s, "baseURL=") && 8 < s.length()) {
		        m_sBaseUrl = s.substr(8, s.length());
		}
		if (StringUtils::StartsWith(s, "xmlURL=") && 7 < s.length()) {
		        m_sXMLUrl = s.substr(7, s.length());
		}
		if (StringUtils::StartsWith(s, "genresToLoad=") && 13 < s.length()) {
		        string genreString = s.substr(13, s.length());
			vector<string> vectGenres;
			StringUtils::Tokenize(genreString, ",", vectGenres);
			for (uint j = 0; j < vectGenres.size(); j++) {
			        m_vectGenresToLoad.push_back(vectGenres[j]);
			}
		}
		if (StringUtils::StartsWith(s, "limitStations=") && 14 < s.length()) {
		        m_iLimit = atoi(s.substr(14, s.length()).c_str());
		}
		if (StringUtils::StartsWith(s, "getInterval=") && 12 < s.length()) {
		        m_iGetInterval = atoi(s.substr(12, s.length()).c_str());
		}
		if (StringUtils::StartsWith(s, "runInterval=") && 12 < s.length()) {
		        m_iRunInterval = atoi(s.substr(12, s.length()).c_str());
		}
	}
	// Sanity check for some values, setting these too low will increase the load on the core
	if (m_iGetInterval < 60) {
	        m_iGetInterval = 60;
	}
	if (m_iRunInterval < 1800) {
	        m_iRunInterval = 1800;
	}
	//start alarm if not already started
	m_pUpdate->type = 'G';
	if(-1 == m_pAlarmManager->FindAlarmByType(UPDATE_SHOUTCAST))
	        m_pAlarmManager->AddRelativeAlarm(m_iGetInterval, this, UPDATE_SHOUTCAST, m_pUpdate);
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Shoutcast_Radio_Plugin::Register()
//<-dceag-reg-e->
{
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	if( !m_pMedia_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to shoutcast radio plugin");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Shoutcast_Radio_Plugin_CONST);  // No media associated with this. we're just going to populate the grids ourselves and maange our own media
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Shoutcast_Radio_Plugin_Command *Create_Shoutcast_Radio_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Shoutcast_Radio_Plugin(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Shoutcast_Radio_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Shoutcast_Radio_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}



//<-dceag-sample-b->!
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Shoutcast_Radio_Plugin::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

class MediaStream *Shoutcast_Radio_Plugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Shoutcast_Radio_Plugin::CreateMediaStream() start");

	return new MediaStream(pMediaHandlerInfo,iPK_MediaProvider,pMediaDevice,iPK_Users,DCE::st_Broadcast,StreamID);
}

bool Shoutcast_Radio_Plugin::StartMedia( class MediaStream *pMediaStream,string &sError )
{
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Shoutcast_Radio_Plugin::StartMedia() start");

	return MediaHandlerBase::StartMedia(pMediaStream, sError);
}

bool Shoutcast_Radio_Plugin::StopMedia( class MediaStream *pMediaStream )
{
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Shoutcast_Radio_Plugin::StopMedia() start");
    
	return MediaHandlerBase::StopMedia(pMediaStream);
}

void Shoutcast_Radio_Plugin::PopulateDataGrid(string sToken,MediaListGrid *pMediaListGrid,int PK_MediaType, string sPK_Attribute, int PK_AttributeType_Sort, bool bShowFiles, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int iLastViewed, int *iPK_Variable, string *sValue_To_Assign )
{
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::PopulateDataGrid() start");

	//TODO: send message to user "Downloading data, please try again soon!" ?
	if (m_vectGenres.empty()) {
	        updateGenres();
	}
	if(sPK_Attribute.empty()==false )
	{
	        string genre = sPK_Attribute;
	        if (m_mapGenreID.find(genre) == m_mapGenreID.end()) {
		        updateStationsForGenre(genre);
		}
	        vector<Station> vectStations;  
		getStationsByGenre(sPK_Attribute, vectStations);
	  
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::PopulateDataGrid() going to add %d items", vectStations.size());
		for (uint i = 0; i < vectStations.size(); i++) {
		        Station station = vectStations[i];
			string url = getStationURL(station.id);
			FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo(station.name,"!E," + sToken + "," + url,0,StringUtils::SQLDateTime("2008-02-8"));
			//  pFileBrowserInfo->m_PK_Picture = 102;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
		return;
	} else {
	        // List genres selected for current user (TODO: add proper user-based listing)
	        for (uint i = 0; i < m_vectGenresToLoad.size(); i++) {
		        string genre = m_vectGenresToLoad[i];
			FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo(genre,"!e," + sToken + ","+genre,0,StringUtils::SQLDateTime("2008-02-10"));
			//pFileBrowserInfo->m_PK_Picture = 100;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::PopulateDataGrid() end");
}

void Shoutcast_Radio_Plugin::GetExtendedAttributes(string sType, string sPK_MediaSource, string sURL, string *sValue_To_Assign)
{
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::GetExtendedAttributes() start, sURL = %s", sURL.c_str());
        uint pos = sURL.find("id=");
	if (pos != string::npos) {
 	        uint endpos = sURL.find("&");
		int id = 0;
		string sID = "";
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::GetExtendedAttributes() pos = %d, endpos = %d", pos, endpos);
		if (endpos != string::npos && endpos < sURL.length()) {
	                sID = sURL.substr(pos+3, endpos-pos-3);
		} else {
		        sID = sURL.substr(pos+3);
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::GetExtendedAttributes() id = %s", sID.c_str());
		id = atoi(sID.c_str());
		if (id > 0) {
		        map<int, Station>::iterator it = m_mapIDStation.find(id);
			if (it != m_mapIDStation.end()) {
		                Station station = it->second;
				*sValue_To_Assign = "FILE\t" + getStationURL(station.id) +
			                "\tTITLE\t" + station.name +
			                "\tSYNOPSIS\t" +
			                "\tPICTURE\t";
			}
		}
	}
}

string Shoutcast_Radio_Plugin::getStationURL(int id) {
        string sURLSuffix = "&file=filename.pls";
	string url = m_sBaseUrl + m_sTuneInBase + "?id=";
	char cid[10];
	sprintf(cid,"%d", id);
	url = url + string(cid) + sURLSuffix;
	return url;
}

string Shoutcast_Radio_Plugin::getLimitURL() {
        if (m_iLimit > 0) {
	  return "&limit=" + StringUtils::itos(m_iLimit);
	} else {
	  return "";
	}
}
vector<string> Shoutcast_Radio_Plugin::getGenres() {
        return m_vectGenres;
}

bool Shoutcast_Radio_Plugin::getStationsByGenre(string genre, vector<Station> &vectStations) {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::getStationsByGenre() start");

        map<string, vector<int> >::iterator it;
	it = m_mapGenreID.find(genre);
	if (it != m_mapGenreID.end()) {
	        getStationById(it->second, vectStations);
		return true;
	}
	return false;
}

void Shoutcast_Radio_Plugin::getStationById(vector<int> vectID, vector<Station> &vectStations) {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::getStationById() start");
        map<int, Station>::iterator it;
	for(uint s=0;s< vectID.size();++s) {
	        it = m_mapIDStation.find(vectID[s]);
		if (it != m_mapIDStation.end()) {
		        vectStations.push_back(it->second);
		}
	}
}

void Shoutcast_Radio_Plugin::updateGenres() {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::updateGenres() start");

	string data;
	string res = HttpGet(m_sBaseUrl + m_sXMLUrl, &data);
	if (res.compare("OK") != 0) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Shoutcast_Radio_Plugin::updateGenres() Http GET FAILED!");
		return;
	}
	xmlTextReaderPtr reader;
	int ret;
	
	reader = xmlReaderForMemory(data.c_str(), data.size(), "", NULL, 0);
	if (reader != NULL) {
	        // go to next node
	        ret = xmlTextReaderRead(reader);

		m_vectGenres.clear();
		while (ret == 1) {
		        if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_END_ELEMENT) {
			        char *attrValue = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*) "name");
				if (attrValue) {
				        m_vectGenres.push_back(string(attrValue));
				}
			}
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
		if (ret != 0) {
		        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Shoutcast_Radio_Plugin::updateGenres() failed to parse");
		}
	} else {
		        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Shoutcast_Radio_Plugin::updateGenres() unable to open");
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Shoutcast_Radio_Plugin::updateGenres() end, genres = %d", m_vectGenres.size());
}

bool Shoutcast_Radio_Plugin::parseStation(xmlTextReaderPtr reader,Station& station) {

        char *name = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*)"name");
	if (name) {
	        station.name = string(name);
	}
	char *id = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*)"id");
	if (id) {
	        station.id = atoi(id);
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::parseStation() name %s, id %s", name, id);

	if (!name || !id) {
	        // require at least id and name
	        return false;
	}
	// bitrate
	char *br = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*)"br");
	if (br) {
	        station.br = atoi(br);
	}
	char *genre = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*)"genre");
	if (genre) {
	        station.genre = string(genre);
	}
	// current track
	char *ct = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*)"ct");
	if (ct) {
	        station.ct = string(ct);
	}
	// listener count
	char *lc = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*)"cl");
	if (lc) {
	        station.lc = atoi(lc);
	}

	return true;
}

void Shoutcast_Radio_Plugin::updateStationsForGenre(string genre) {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::updateStationsForGenre() start, genre = %s", genre.c_str());

        string url = m_sBaseUrl + m_sXMLUrl + "?genre=" + genre + getLimitURL();
	string data;
	string res = HttpGet(url, &data);
	if (res.compare("OK") != 0) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Shoutcast_Radio_Plugin::updateStationsForGenres() Http GET FAILED!");
		return;
	}
	xmlTextReaderPtr reader;
	int ret;

	reader = xmlReaderForMemory(data.c_str(), data.size(), "", NULL, 0);
	if (reader != NULL) {
	        // go to next node
	        ret = xmlTextReaderRead(reader);
		vector<int> vectGenreIDs;
		while (ret == 1) {
		  if (xmlTextReaderNodeType(reader) != XML_READER_TYPE_END_ELEMENT) {
		    char* nodeName = (char*) xmlTextReaderConstName(reader);
		    if (nodeName) {
		      if (strcmp(nodeName, "tunein") == 0) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::updateStationsForGenre() nodeName is tunein ");
			char *tunein = (char*) xmlTextReaderGetAttribute(reader, (xmlChar*)"base");
			if (tunein) {
			  m_sTuneInBase = string(tunein);
			  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shoutcast_Radio_Plugin::updateStationsForGenre() m_sTuneInBase = %s", m_sTuneInBase.c_str());
			}
		      } else if (strcmp(nodeName, "station") == 0) {
			Station station;
			if (parseStation(reader, station)) {
			  m_mapIDStation[station.id] = station;
			  vectGenreIDs.push_back(station.id);
			}
		      }
		    }
		  }
		  ret = xmlTextReaderRead(reader);
		}
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Shoutcast_Radio_Plugin::updateStationsForGenre() genre = %s, vectGenreIDs.size = %d", genre.c_str(),  vectGenreIDs.size());

		m_mapGenreID[genre] = vectGenreIDs;
		xmlFreeTextReader(reader);
		if (ret != 0) {
		  fprintf(stderr, "%s : failed to parse\n", url.c_str());
		}
	} else {
	        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Shoutcast_Radio_Plugin::updateStationsForGenre() unable to create xml reader");
	}

}

void Shoutcast_Radio_Plugin::AlarmCallback(int id, void* param) {
        ShoutcastUpdate* pUpdate = (ShoutcastUpdate*)param;
	if (pUpdate != NULL) {
	        if (pUpdate->type == 'G') {
		        updateGenres();

			// schedule update of stations
		        pUpdate->genre = 0;
			pUpdate->type = 'S';
			m_pAlarmManager->AddRelativeAlarm(m_iGetInterval, this, UPDATE_SHOUTCAST, pUpdate);

		} else if (pUpdate->type == 'S') {
		        string genre = m_vectGenresToLoad[pUpdate->genre];
			if (!genre.empty()) {
			        updateStationsForGenre(genre);
				// TODO: retry if failed
			}
			// schedule update of stations for next genre
			if (pUpdate->genre < m_vectGenresToLoad.size()-1) {
			        pUpdate->genre++;
				m_pAlarmManager->AddRelativeAlarm(m_iGetInterval, this, UPDATE_SHOUTCAST, pUpdate);
			} else {
			  // schedule next genre update
			  pUpdate->genre = 0;
			  pUpdate->type = 'G';
			  m_pAlarmManager->AddRelativeAlarm(m_iRunInterval, this, UPDATE_SHOUTCAST, pUpdate);
			}
		}
	}
}
