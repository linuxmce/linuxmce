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
#include "Game_PlugIn.h"
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
Game_PlugIn::Game_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Game_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->

      , m_GameMediaMutex("mame media mutex")

{

	m_GameMediaMutex.Init(NULL);
	m_pAlarmManager = NULL;
	m_pGAMEROM = NULL;
	m_dwPK_File_LastCheckedForNewRecordings=0;

}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Game_PlugIn::~Game_PlugIn()
//<-dceag-dest-e->
{
	{
		PLUTO_SAFETY_LOCK( mm, m_GameMediaMutex );
	}
 
	pthread_mutex_destroy(&m_GameMediaMutex.mutex);
}

//<-dceag-getconfig-b->
bool Game_PlugIn::GetConfig()
{
	if( !Game_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	m_pAlarmManager = new AlarmManager();
        m_pAlarmManager->Start(2);      //4 = number of worker threads
 
	m_pGAMEROM = new(GAMEROM);
	m_pGAMEROM->InitDatabase();

	return true;
}

void Game_PlugIn::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pGAMEROM->CloseDatabase();
	delete m_pGAMEROM;
	m_pGAMEROM = NULL;
	m_pAlarmManager = NULL;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Game_PlugIn::Register()
//<-dceag-reg-e->
{

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Starting Game_Plugin::Register()");

	m_iPriority=DATA_Get_Priority();
 
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to game plugin");
		return false;
	}
 
	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Game_Player_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Registered device %d",DEVICETEMPLATE_Game_Player_CONST);

	/* Register my Alert Plugin */
 	m_pAlarmManager->AddRelativeAlarm(30,this,CHECK_FOR_NEW_ROMS,NULL);

	RegisterMsgInterceptor(( MessageInterceptorFn )( &Game_PlugIn::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );

	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
        cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
        can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Game_PlugIn_Command *Create_Game_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
        return new Game_PlugIn(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Game_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Game_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Game_PlugIn::SomeFunction()
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

class MediaStream *Game_PlugIn::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CreateMediaStream Called");
 
	GameMediaStream *pGameMediaStream;
	MediaDevice *pMediaDevice_PassedIn;
 
	PLUTO_SAFETY_LOCK( xm, m_GameMediaMutex );
 
	if(m_bQuit_get())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Game_PlugIn::CreateMediaStream with m_bQuit");
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
		 pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_Game_Player_CONST )
	{
		pMediaDevice_PassedIn = pMediaDevice;
		pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_Game_Player_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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
 
	pGameMediaStream = new GameMediaStream( this, pMediaHandlerInfo,iPK_MediaProvider,
							pMediaDevice,
							iPK_Users, st_RemovableMedia, StreamID );
 
	m_mapDevicesToStreams[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device] = StreamID;

	return pGameMediaStream;
 
}

bool Game_PlugIn::StartMedia( MediaStream *pMediaStream,string &sError )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Game_PlugIn::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
 
	GameMediaStream *pGameMediaStream = NULL;
	if ( (pGameMediaStream = ConvertToGameMediaStream(pMediaStream, "Game_PlugIn::StartMedia(): ")) == NULL )
		return false;
 
	string sFileToPlay;
	MediaFile *pMediaFile = NULL;

	if (pGameMediaStream->m_dequeMediaFile.size()>pGameMediaStream->m_iDequeMediaFile_Pos)
	  pMediaFile = pGameMediaStream->m_dequeMediaFile[pGameMediaStream->m_iDequeMediaFile_Pos];
 
	sFileToPlay = pGameMediaStream->GetFilenameToPlay("Empty file name");
 
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Game_PlugIn::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());
 
	string mediaURL;
	string Response;
 
	mediaURL = sFileToPlay;
 
	// send the CMD straight through.
 
	// Fill out the stream bits for OSD
	string sROMName = FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(mediaURL));

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Game_PlugIn::CreateMediaStream(): Setting m_sMediaDescription to: %s",
							m_pGAMEROM->getTitleForROM(sROMName).c_str());
 
	// pGameMediaStream->m_sMediaDescription = m_pGAMEROM->getTitleForROM(sROMName);
    	// pGameMediaStream->m_sSectionDescription = m_pGAMEROM->getManufacturerForROM(sROMName) + " (" + m_pGAMEROM->getYearForROM(sROMName) + ")";
    	// pGameMediaStream->m_sMediaSynopsis = "Example Synopsis!";


///////////////////////////////////////////////////////////////////////
// Game System specific code here. Basically, if we need to
// we set the window name.
// yes, i know this is a hack. hopefully it will be replaced later.
// -tschak
///////////////////////////////////////////////////////////////////////

	// First, assume MAME, unless otherwise changed here.
	pGameMediaStream->m_sAppName = "mame.mame";
	pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_CONST;

	if (mediaURL.find("/a2600") != string::npos || StringUtils::ToLower(mediaURL).find(".a26") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_a2600_CONST;
	}

	if (mediaURL.find("/a5200") != string::npos || StringUtils::ToLower(mediaURL).find(".a52") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_a5200_CONST;
	}

	if (mediaURL.find("/a7800") != string::npos || StringUtils::ToLower(mediaURL).find(".a78") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_a7800_CONST;
	}

	if (mediaURL.find("/coleco") != string::npos || StringUtils::ToLower(mediaURL).find(".col") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_coleco_CONST;
	}

	if (mediaURL.find("/intv") != string::npos || StringUtils::ToLower(mediaURL).find(".int") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_intv_CONST;
	}

	if (mediaURL.find("/sg1000") != string::npos || StringUtils::ToLower(mediaURL).find(".sg") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_sg1000_CONST;
	}

	if (mediaURL.find("/sms") != string::npos || StringUtils::ToLower(mediaURL).find(".sms") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_sms_CONST;
	}

	if (mediaURL.find("/nes") != string::npos || StringUtils::ToLower(mediaURL).find(".nes") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_nes_CONST;
	}

	if (mediaURL.find("/famicom") != string::npos || StringUtils::ToLower(mediaURL).find(".fam") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_famicom_CONST;
	}

	if (mediaURL.find("/snespal") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_snespal_CONST;
	}
        if (mediaURL.find("/snes") != string::npos || StringUtils::ToLower(mediaURL).find(".smc") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".sfc") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".fig") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".swc") != string::npos)
        {
                pGameMediaStream->m_sAppName = "mess.mess";
                pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_snes_CONST; 
        }

	if (mediaURL.find("/genesis") != string::npos || 
		StringUtils::ToLower(mediaURL).find(".smd") != string::npos || 
		StringUtils::ToLower(mediaURL).find(".md") != string::npos ||
		StringUtils::ToLower(mediaURL).find(".gen") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_genesis_CONST; 
	}

	if (mediaURL.find("/tg16") != string::npos 
		&& StringUtils::ToLower(mediaURL).find(".pce") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_tg16_CONST;
	}

        if (mediaURL.find("/pce") != string::npos
                && StringUtils::ToLower(mediaURL).find(".pce") != string::npos)
        {
                pGameMediaStream->m_sAppName = "mess.mess";
                pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_pce_CONST;
        }

        if (mediaURL.find("/sgx") != string::npos
                && StringUtils::ToLower(mediaURL).find(".pce") != string::npos)
        {
                pGameMediaStream->m_sAppName = "mess.mess";
                pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_sgx_CONST;
        }

	if (mediaURL.find("/vectrex") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".vec") != string::npos 
		|| StringUtils::ToLower(mediaURL).find(".gam") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_vectrex_CONST;
	}

        if (mediaURL.find("/apple2") != string::npos
                || StringUtils::ToLower(mediaURL).find(".dsk") != string::npos
                || StringUtils::ToLower(mediaURL).find(".nib") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".po") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".do") != string::npos)
        {
                pGameMediaStream->m_sAppName = "mess.mess";
                pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_apple2_CONST;
        }

	if (mediaURL.find("/vic20") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".20") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".40") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".60") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".70") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".a0") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".b0") != string::npos)
	{
		pGameMediaStream->m_sAppName = "xvic.xvic";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_vic20_CONST;
	}

	if (mediaURL.find("/c64") != string::npos 
		|| StringUtils::ToLower(mediaURL).find(".p00") != string::npos 
		|| StringUtils::ToLower(mediaURL).find(".prg") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".t64") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".g64") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".d64") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".crt") != string::npos)
	{
		pGameMediaStream->m_sAppName = "x64.x64";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_c64_CONST;
	}

	if (mediaURL.find("/a800") != string::npos
	    || mediaURL.find("/a130xe") != string::npos
	    || StringUtils::ToLower(mediaURL).find(".atr") != string::npos
	    || StringUtils::ToLower(mediaURL).find(".xfd") != string::npos
	    || StringUtils::ToLower(mediaURL).find(".rom") != string::npos)
	  {
	    pGameMediaStream->m_sAppName = "mess.mess";
	    pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_Atari800_CONST;
	  }

	if (mediaURL.find(".j64") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mess.mess";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_jaguar_CONST;
	}

	if (mediaURL.find("/ps1") != string::npos)
	{
		pGameMediaStream->m_sAppName = "pcsx.pcsx";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_ps1_CONST;
	}

	if (mediaURL.find("/ps2") != string::npos)
	{
                pGameMediaStream->m_sAppName = "pcsx2.pcsx2";
                pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_ps2_CONST;
	}
	
	DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,
					   pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					   pGameMediaStream->m_iPK_MediaType,
					   pGameMediaStream->m_iStreamID_get( ),
					   pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pGameMediaStream->m_sStartPosition,
					   mediaURL);

	SendCommand(CMD_Play_Media);

	if (pMediaFile && pGameMediaStream->m_iPK_Playlist==0)
	  pMediaFile->m_sStartPosition="";
 
	/** We're going to send a message to all the orbiters in this area so they know what the remote is,
	and we will send all bound remotes to the new screen */
	for( MapEntertainArea::iterator itEA = pGameMediaStream->m_mapEntertainArea.begin( );itEA != pGameMediaStream->m_mapEntertainArea.end( );++itEA )
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
			pGameMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
		}
	}

	return MediaHandlerBase::StartMedia(pMediaStream,sError);
 
}

bool Game_PlugIn::StopMedia( class MediaStream *pMediaStream )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StopMedia Called");
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    if( it!=m_mapDevicesToStreams.end() )
        m_mapDevicesToStreams.erase(it);

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Game_Plugin::StopMedia() Stopping Media Stream Playback... Pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
 
	GameMediaStream *pGameMediaStream = NULL;
 
	if ((pGameMediaStream = ConvertToGameMediaStream(pMediaStream,"Game_Plugin::StopMedia():")) == NULL )
		return false;
 
	if (!pGameMediaStream->m_pMediaDevice_Source)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Stopping media in Game_Plugin but MediaDevice_Source is NULL");
	    return false;
	  }

	int PK_Device = pGameMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int iStreamID = pGameMediaStream->m_iStreamID_get();
	string savedPosition;
 
	DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,
						pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
						pGameMediaStream->m_iStreamID_get(),
						&savedPosition);
 
	string sResponse;
	if (!SendCommand(CMD_Stop_Media))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"The Target device %d didn't respond to stop media command!", PK_Device);
	  }
	else
	  {
	    pGameMediaStream->m_sLastPosition=savedPosition;
	    if (pGameMediaStream->m_iDequeMediaFile_Pos>=0 && pGameMediaStream->m_iDequeMediaFile_Pos<pGameMediaStream->m_dequeMediaFile.size())
	      {
		pGameMediaStream->m_dequeMediaFile[pGameMediaStream->m_iDequeMediaFile_Pos]->m_sStartPosition = savedPosition;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media Stopped at %s",savedPosition.c_str());
	      }
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"The target device %d responded to stop media comman", pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	  }
 
	return MediaHandlerBase::StopMedia(pMediaStream);

}

MediaDevice *Game_PlugIn::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_Game_Player_CONST);
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for a proper device in the ent area %d (%s)", pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_sDescription.c_str());
	if ( pMediaDevice == NULL )
	{

		return NULL;
	}
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
 
	return pMediaDevice;
}

GameMediaStream *Game_PlugIn::ConvertToGameMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}
 
	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_GAME )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a GameMediaStream!").c_str());
		return NULL;
	}
 
	return static_cast<GameMediaStream*>(pMediaStream);
}

void Game_PlugIn::AlarmCallback(int id, void* param) 
{
	//no longer appropriate.
	//if (id==CHECK_FOR_NEW_ROMS) {
	//	Game_PlugIn::CheckForNewROMs();	
	//}
}

void Game_PlugIn::CheckForNewROMs() 
{
 
//	// Scan for new roms, and tag things appropriately. This is run by the AlarmManager that we instantiate.
/* 
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
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Game_PlugIn::CheckForNewROMs PK_File_Max %d m_dwPK_File_LastCheckedForNewRecordings %d files: %d %s",
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

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Title: %s",m_pGAMEROM->getTitleForROM(sMyRomName).c_str());  
		m_pMedia_Plugin->CMD_Add_Media_Attribute(m_pGAMEROM->getTitleForROM(sMyRomName),0,"",ATTRIBUTETYPE_Title_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Title);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Year: %s",m_pGAMEROM->getYearForROM(sMyRomName).c_str());
		m_pMedia_Plugin->CMD_Add_Media_Attribute(m_pGAMEROM->getYearForROM(sMyRomName),0,"",ATTRIBUTETYPE_Year_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Year);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Manufacturer: %s",m_pGAMEROM->getTitleForROM(sMyRomName).c_str());
		m_pMedia_Plugin->CMD_Add_Media_Attribute(m_pGAMEROM->getManufacturerForROM(sMyRomName),0,"",ATTRIBUTETYPE_Manufacturer_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Manufacturer);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Genre: %s",m_pGAMEROM->getTitleForROM(sMyRomName).c_str());
		m_pMedia_Plugin->CMD_Add_Media_Attribute(m_pGAMEROM->getGenreForROM(sMyRomName),0,"",ATTRIBUTETYPE_Genre_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Genre);
 
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
	m_pAlarmManager->CancelAlarmByType(CHECK_FOR_NEW_ROMS);  // Don't get multiple entries in here
	m_pAlarmManager->AddRelativeAlarm(180,this,CHECK_FOR_NEW_ROMS,NULL);  // check again in a few mins 
 
	m_dwPK_File_LastCheckedForNewRecordings = PK_File_Max;
*/ 
}

/* Grabbed almost verbatim from Xine Player */
bool Game_PlugIn::MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	/** Confirm this is from one of ours */
	if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate != DEVICETEMPLATE_Game_Player_CONST )
		return false; 

	int StreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	bool bOnOff = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";

	/** Find the stream */
	GameMediaStream *pGameMediaStream = NULL;
	MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( StreamID,pMessage->m_dwPK_Device_From );
	if( !pMediaStream || (pGameMediaStream = ConvertToGameMediaStream(pMediaStream, "Game_Plugin::MenuOnScreen(): ")) == NULL )
		return false;

	pGameMediaStream->m_bUseAltScreens=bOnOff;

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream %p with id %d and type %d reached an OnScreen Menu.", pGameMediaStream, pGameMediaStream->m_iStreamID_get( ), pGameMediaStream->m_iPK_MediaType );
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream m_mapEntertainArea.size( ) %d", pGameMediaStream->m_mapEntertainArea.size( ) );


	/** We're going to send a message to all the orbiters in this area so they know what the remote is,
	and we will send all bound remotes to the new screen */
	for( MapEntertainArea::iterator itEA = pGameMediaStream->m_mapEntertainArea.begin( );itEA != pGameMediaStream->m_mapEntertainArea.end( );++itEA )
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
			pGameMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
		}
	}

	return false;
}
