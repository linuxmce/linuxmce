/*
     Copyright (c) 2008 LOCALE|concept
 
     www.localeconcept.com
 
     Phone: +1 (617) 319-8219
 
 
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 
     See the GNU General Public License for more details.
 
*/
//<-dceag-d-b->
#include "MAME_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
 
#include <iostream>
using namespace std;
using namespace DCE;
 
#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
 
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Table_Users.h"
#include "../pluto_media/Table_LongAttribute.h"
#include "../pluto_media/Table_MediaProvider.h"
#include "../pluto_media/Table_ProviderSource.h"
#include "../pluto_media/Table_Picture.h"
#include "../pluto_media/Define_FileFormat.h"
#include "../pluto_media/Define_MediaSubType.h"
#include "../pluto_media/Define_AttributeType.h"
 
//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MAME_PlugIn::MAME_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MAME_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
		, m_MAMEMediaMutex("mame media mutex")
 
{
 
	m_MAMEMediaMutex.Init(NULL);
	m_pAlarmManager = NULL;
	m_pMAMEROM = NULL;
	m_dwPK_File_LastCheckedForNewRecordings=0;
 
}
 
//<-dceag-dest-b->
MAME_PlugIn::~MAME_PlugIn()
//<-dceag-dest-e->
{
 
	{
		PLUTO_SAFETY_LOCK( mm, m_MAMEMediaMutex );
	}
 
	pthread_mutex_destroy(&m_MAMEMediaMutex.mutex);
 
}
 
//<-dceag-getconfig-b->
bool MAME_PlugIn::GetConfig()
{
	if( !MAME_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
 
	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
 
	m_pAlarmManager = new AlarmManager();
        m_pAlarmManager->Start(2);      //4 = number of worker threads
 
	m_pMAMEROM = new(MAMEROM);
	m_pMAMEROM->InitDatabase();
 
	return true;
}
 
void MAME_PlugIn::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	delete m_pMAMEROM;
	m_pMAMEROM = NULL;
	m_pAlarmManager = NULL;
}
 
 
//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MAME_PlugIn::Register()
//<-dceag-reg-e->
{
 
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"REGISTER!");
 
	m_iPriority=DATA_Get_Priority();
 
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to xine plugin");
		return false;
	}
 
	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_MAME_Player_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );
 
	/* Register my Alert Plugin */
 	m_pAlarmManager->AddRelativeAlarm(30,this,CHECK_FOR_NEW_ROMS,NULL);

	RegisterMsgInterceptor(( MessageInterceptorFn )( &MAME_PlugIn::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );

	return Connect(PK_DeviceTemplate_get()); 
 
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
void MAME_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void MAME_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{

	sCMD_Result = "UNKNOWN COMMAND";
}
 
//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****
 
**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here
 
void MAME_PlugIn::SomeFunction()
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
 
 
class MediaStream *MAME_PlugIn::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CreateMediaStream Called");
 
	MAMEMediaStream *pMAMEMediaStream;
	MediaDevice *pMediaDevice_PassedIn;
 
	PLUTO_SAFETY_LOCK( xm, m_MAMEMediaMutex );
 
	if(m_bQuit_get())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MAME_PlugIn::CreateMediaStream with m_bQuit");
		return NULL;
	}
 
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	pMediaDevice_PassedIn = NULL;
	if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}
 
	if ( pMediaDevice != NULL  && // test the media device only if it set
		 pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_MAME_Player_CONST )
	{
		pMediaDevice_PassedIn = pMediaDevice;
		pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_MAME_Player_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
	}
 
	if ( !pMediaDevice )
	{
		for(size_t s=0;s<vectEntertainArea.size();++s)
		{
			EntertainArea *pEntertainArea = vectEntertainArea[0];
			pMediaDevice = FindMediaDeviceForEntertainArea(pEntertainArea);
			if( pMediaDevice )
				break;
		}
		if( !pMediaDevice )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I didn't find a device in the target ent area.");
			return NULL;
		}
	}
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Selected device (%d: %s) as playback device!",
			pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
			pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
 
	pMAMEMediaStream = new MAMEMediaStream( this, pMediaHandlerInfo,iPK_MediaProvider,
							pMediaDevice,
							iPK_Users, st_RemovableMedia, StreamID );
 
	m_mapDevicesToStreams[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device] = StreamID;

	return pMAMEMediaStream;
 
}
 
bool MAME_PlugIn::StartMedia( MediaStream *pMediaStream,string &sError )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StartMedia Called");
 
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MAME_PlugIn::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
 
	MAMEMediaStream *pMAMEMediaStream = NULL;
	if ( (pMAMEMediaStream = ConvertToMAMEMediaStream(pMediaStream, "MAME_PlugIn::StartMedia(): ")) == NULL )
		return false;
 
	string sFileToPlay;
	MediaFile *pMediaFile = NULL;
 
	sFileToPlay = pMAMEMediaStream->GetFilenameToPlay("Empty file name");
 
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MAME_PlugIn::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());
 
	string mediaURL;
	string Response;
 
	mediaURL = sFileToPlay;
 
	// send the CMD straight through.
 
	// Fill out the stream bits for OSD
	string sROMName = FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(mediaURL));

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MAME_PlugIn::CreateMediaStream(): Setting m_sMediaDescription to: %s",
							m_pMAMEROM->getTitleForROM(sROMName).c_str());
 
	pMAMEMediaStream->m_sMediaDescription = m_pMAMEROM->getTitleForROM(sROMName);
    	pMAMEMediaStream->m_sSectionDescription = m_pMAMEROM->getManufacturerForROM(sROMName) + " (" + m_pMAMEROM->getYearForROM(sROMName) + ")";
    	pMAMEMediaStream->m_sMediaSynopsis = "Example Synopsis!";
 
	DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,
						pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
						pMAMEMediaStream->m_iPK_MediaType,
						pMAMEMediaStream->m_iStreamID_get( ),
						"00:00:00",mediaURL);
	SendCommand(CMD_Play_Media);
 
	/** We're going to send a message to all the orbiters in this area so they know what the remote is,
	and we will send all bound remotes to the new screen */
	for( MapEntertainArea::iterator itEA = pMAMEMediaStream->m_mapEntertainArea.begin( );itEA != pMAMEMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
				continue;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			bool bBound = pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)!=pEntertainArea->m_mapBoundRemote.end();
			pMAMEMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
		}
	}

	return MediaHandlerBase::StartMedia(pMediaStream,sError);
 
}
 
bool MAME_PlugIn::StopMedia( class MediaStream *pMediaStream )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StopMedia Called");
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    if( it!=m_mapDevicesToStreams.end() )
        m_mapDevicesToStreams.erase(it);

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MAME_Plugin::StopMedia() Stopping Media Stream Playback... Pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
 
	MAMEMediaStream *pMAMEMediaStream = NULL;
 
	if ((pMAMEMediaStream = ConvertToMAMEMediaStream(pMediaStream,"MAME_Plugin::StopMedia():")) == NULL )
		return false;
 
	string savedPosition;
 
	DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,
						pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
						pMAMEMediaStream->m_iStreamID_get(),
						&savedPosition);
 
	SendCommand(CMD_Stop_Media);
 
	return MediaHandlerBase::StopMedia(pMediaStream);
}
 
MediaDevice *MAME_PlugIn::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_MAME_Player_CONST);
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for a proper device in the ent area %d (%s)", pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_sDescription.c_str());
	if ( pMediaDevice == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not find a Xine Player device (with device template id: %d) in the entertainment area: %d. Looking for a squeeze box.",
				DEVICETEMPLATE_MAME_Player_CONST,
				pEntertainArea->m_iPK_EntertainArea);
 
		return NULL;
	}
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
 
	return pMediaDevice;
}
 
MAMEMediaStream *MAME_PlugIn::ConvertToMAMEMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}
 
	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_GAME )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a MAMEMediaStream!").c_str());
		return NULL;
	}
 
	return static_cast<MAMEMediaStream*>(pMediaStream);
}
 
void MAME_PlugIn::AlarmCallback(int id, void* param) 
{
	if (id==CHECK_FOR_NEW_ROMS) {
		MAME_PlugIn::CheckForNewROMs();	
	}
}
 
void MAME_PlugIn::CheckForNewROMs() 
{
 
//	// Scan for new roms, and tag things appropriately. This is run by the AlarmManager that we instantiate.
 
	string sSQL = "SELECT max(PK_File) FROM File";
	PlutoSqlResult result;
   	DB_ROW row;
 
	int PK_File_Max=0;
	if( ( result.r=m_pMedia_Plugin->m_pDatabase_pluto_media->db_wrapper_query_result( sSQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) && row[0] )
		PK_File_Max = atoi(row[0]);
 
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex); // protect m_sNewRecordings
	sSQL = "LEFT JOIN File_Attribute ON FK_File=PK_File WHERE (Missing=0 AND Path like '%games%' AND FK_File IS NULL AND PK_File>" + StringUtils::itos(m_dwPK_File_LastCheckedForNewRecordings) + ")";
	vector<Row_File *> vectRow_File;
	m_pMedia_Plugin->m_pDatabase_pluto_media->File_get()->GetRows( sSQL, &vectRow_File );
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MAME_PlugIn::CheckForNewROMs PK_File_Max %d m_dwPK_File_LastCheckedForNewRecordings %d files: %d %s",
		PK_File_Max, m_dwPK_File_LastCheckedForNewRecordings, (int) vectRow_File.size(), sSQL.c_str());
 
	for(vector<Row_File *>::iterator it=vectRow_File.begin();it!=vectRow_File.end();++it)
	{

		Row_File *pRow_File = *it;
		// For each file, let's grab the necessary bits of tagging.
 
		// pRow_File->Filename_get()
 
		string sMyRomName = FileUtils::FileWithoutExtension(pRow_File->Filename_get());
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ROM Name is %s",sMyRomName.c_str()); 

		int PK_Attribute_Title 		= 0;
		int PK_Attribute_Year 		= 0;
		int PK_Attribute_Manufacturer 	= 0;
		int PK_Attribute_Genre 		= 0;

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Title: %s",m_pMAMEROM->getTitleForROM(sMyRomName).c_str());  
		m_pMedia_Plugin->CMD_Add_Media_Attribute(m_pMAMEROM->getTitleForROM(sMyRomName),0,"",ATTRIBUTETYPE_Title_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Title);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Year: %s",m_pMAMEROM->getYearForROM(sMyRomName).c_str());
		m_pMedia_Plugin->CMD_Add_Media_Attribute(m_pMAMEROM->getYearForROM(sMyRomName),0,"",ATTRIBUTETYPE_Year_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Year);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Manufacturer: %s",m_pMAMEROM->getTitleForROM(sMyRomName).c_str());
		m_pMedia_Plugin->CMD_Add_Media_Attribute(m_pMAMEROM->getManufacturerForROM(sMyRomName),0,"",ATTRIBUTETYPE_Manufacturer_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Manufacturer);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Genre: %s",m_pMAMEROM->getTitleForROM(sMyRomName).c_str());
		m_pMedia_Plugin->CMD_Add_Media_Attribute(m_pMAMEROM->getGenreForROM(sMyRomName),0,"",ATTRIBUTETYPE_Genre_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Genre);
 
		if( PK_Attribute_Title ) 
		{
		  int iData_Size = 0;
	          size_t nSize = 0;
		  char *pData;
	          pData = FileUtils::ReadFileIntoBuffer("/home/snap/"+FileUtils::FileWithoutExtension(pRow_File->Filename_get())+".jpeg", nSize);
		  iData_Size = int(nSize);
		  Row_Picture *p_MyPic = m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(pData,iData_Size,"JPG","");
		  if (p_MyPic)
		  { 
		    m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToAttribute(PK_Attribute_Title,p_MyPic->PK_Picture_get());
		    m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToFile(pRow_File->PK_File_get(),p_MyPic->PK_Picture_get());
		  }
		}

	}
 
	m_pMedia_Plugin->m_pDatabase_pluto_media->File_get()->Commit();
	m_pAlarmManager->CancelAlarmByType(CHECK_FOR_NEW_ROMS);  // Don't get multiple entries in here since this is called by both a timer and new recordings
	m_pAlarmManager->AddRelativeAlarm(3600,this,CHECK_FOR_NEW_ROMS,NULL);  /* check again in an hour */
 
	m_dwPK_File_LastCheckedForNewRecordings = PK_File_Max;
 
}
// added commands 2008-02-14 - tschak

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

/* 
void MAME_PlugIn::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
} 
*/

//<-dceag-c185-b->

	/** @brief COMMAND: #185 - Schedule Recording */
	/** This will schedule a recording. */
		/** @param #14 Type */
			/** The type of recording: O=Once, C=Channel */
		/** @param #39 Options */
			/** Options for this recording, tbd later */
		/** @param #68 ProgramID */
			/** The program which will need to be recorded. (The format is defined by the device which created the original datagrid) */

void MAME_PlugIn::CMD_Schedule_Recording(string sType,string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c185-e->
{
	cout << "Need to implement command #185 - Schedule Recording" << endl;
	cout << "Parm #14 - Type=" << sType << endl;
	cout << "Parm #39 - Options=" << sOptions << endl;
	cout << "Parm #68 - ProgramID=" << sProgramID << endl;
}

//<-dceag-c698-b->

	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

void MAME_PlugIn::CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c698-e->
{
	cout << "Need to implement command #698 - Get Extended Media Data" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #68 - ProgramID=" << sProgramID << endl;
}

//<-dceag-c764-b->

	/** @brief COMMAND: #764 - Set Active Menu */
	/** Indicate which menu is active, options are:
live, nonlive, osd */
		/** @param #9 Text */
			/** The menu currently active */

void MAME_PlugIn::CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c764-e->
{
	cout << "Need to implement command #764 - Set Active Menu" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
 
}

//<-dceag-c824-b->

	/** @brief COMMAND: #824 - Sync Providers and Cards */
	/** Synchronize settings for pvr cards and provders */
		/** @param #2 PK_Device */
			/** If specified, this is the capture card that triggered this change to call checktvproviders for */
		/** @param #198 PK_Orbiter */
			/** If specified, this is the orbiter to notify of the progress if this results in scanning for channels */

void MAME_PlugIn::CMD_Sync_Providers_and_Cards(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c824-e->
{
	cout << "Need to implement command #824 - Sync Providers and Cards" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #198 - PK_Orbiter=" << iPK_Orbiter << endl;
}

//<-dceag-c846-b->

	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail the current frame */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

void MAME_PlugIn::CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c846-e->
{
	cout << "Need to implement command #846 - Make Thumbnail" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
}

//<-dceag-c910-b->

	/** @brief COMMAND: #910 - Reporting EPG Status */
	/** Reporting the status of an EPG update */
		/** @param #9 Text */
			/** Any messages about this */
		/** @param #40 IsSuccessful */
			/** true if the process succeeded */
		/** @param #257 Task */
			/** The type of EPG task: channel (retrieving channels), guide (retrieving guide) */

void MAME_PlugIn::CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask,string &sCMD_Result,Message *pMessage)
//<-dceag-c910-e->
{
	cout << "Need to implement command #910 - Reporting EPG Status" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #40 - IsSuccessful=" << bIsSuccessful << endl;
	cout << "Parm #257 - Task=" << sTask << endl;
}

//<-dceag-c911-b->

	/** @brief COMMAND: #911 - Remove Scheduled Recording */
	/** Remove a scheduled recording */
		/** @param #10 ID */
			/** The ID of the recording rule to remove.  This will remove all recordings with this ID, and ProgramID is ignored if this is specified. */
		/** @param #68 ProgramID */
			/** The ID of the program to remove.  If ID is empty, remove just this program. */

void MAME_PlugIn::CMD_Remove_Scheduled_Recording(string sID,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c911-e->
{
	cout << "Need to implement command #911 - Remove Scheduled Recording" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #68 - ProgramID=" << sProgramID << endl;
}

/* Grabbed almost verbatim from Xine Player */
bool MAME_PlugIn::MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	/** Confirm this is from one of ours */
	if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate != DEVICETEMPLATE_MAME_Player_CONST )
		return false; 

	int StreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	bool bOnOff = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";

	/** Find the stream */
	MAMEMediaStream *pMAMEMediaStream = NULL;
	MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( StreamID,pMessage->m_dwPK_Device_From );
	if( !pMediaStream || (pMAMEMediaStream = ConvertToMAMEMediaStream(pMediaStream, "MAME_Plugin::MenuOnScreen(): ")) == NULL )
		return false;

	pMAMEMediaStream->m_bUseAltScreens=bOnOff;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream %p with id %d and type %d reached an OnScreen Menu.", pMAMEMediaStream, pMAMEMediaStream->m_iStreamID_get( ), pMAMEMediaStream->m_iPK_MediaType );
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream m_mapEntertainArea.size( ) %d", pMAMEMediaStream->m_mapEntertainArea.size( ) );


	/** We're going to send a message to all the orbiters in this area so they know what the remote is,
	and we will send all bound remotes to the new screen */
	for( MapEntertainArea::iterator itEA = pMAMEMediaStream->m_mapEntertainArea.begin( );itEA != pMAMEMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
				continue;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			bool bBound = pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)!=pEntertainArea->m_mapBoundRemote.end();
			pMAMEMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
		}
	}

	return false;
}

//<-dceag-c931-b->


