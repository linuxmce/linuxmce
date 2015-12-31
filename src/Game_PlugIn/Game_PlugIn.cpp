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
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);

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
	RegisterMsgInterceptor(( MessageInterceptorFn )( &Game_PlugIn::PlaybackCompleted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Completed_CONST );

	/* Register swap disks datagrid */
	m_pDatagrid_Plugin->RegisterDatagridGenerator(new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Game_PlugIn::SwapDisksGrid ))
						      , DATAGRID_Swap_Disks_CONST,PK_DeviceTemplate_get() );
	
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

//<-dceag-sample-b->!

/**
 * Swap Disks Datagrid
 */

class DataGridTable *Game_PlugIn::SwapDisksGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
  DataGridTable *pDataGrid = new DataGridTable();
  
  vector<EntertainArea *> vectEntertainArea;
  m_pMedia_Plugin->DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
  if( vectEntertainArea.size()==0 || vectEntertainArea[0]->m_pMediaStream==NULL )
    return pDataGrid;
  MediaStream *pMediaStream = vectEntertainArea[0]->m_pMediaStream;
  string Data = GetCurrentDeviceData( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, DEVICEDATA_Disks_CONST );
  string::size_type pos=0;
  int iCurrent = atoi(StringUtils::Tokenize(Data,"\n",pos).c_str());
  for(int i=0;pos!=string::npos && pos<Data.size();++i)
    {
      DataGridCell *pCell = new DataGridCell( StringUtils::Tokenize(Data,"\n",pos), StringUtils::itos(i) );
      pDataGrid->SetData(0,i,pCell);
    }
  *iPK_Variable=VARIABLE_Misc_Data_2_CONST;
  *sValue_To_Assign=StringUtils::itos(iCurrent);
  return pDataGrid;
  
}

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
	string sSystemConfiguration;

	if (pGameMediaStream->m_dequeMediaFile.size()>pGameMediaStream->m_iDequeMediaFile_Pos)
	  pMediaFile = pGameMediaStream->m_dequeMediaFile[pGameMediaStream->m_iDequeMediaFile_Pos];
 
	sFileToPlay = pGameMediaStream->GetFilenameToPlay("Empty file name");
	
	if (pMediaFile->m_mapPK_Attribute.find(ATTRIBUTETYPE_System_Configuration_CONST) != pMediaFile->m_mapPK_Attribute.end())
	  {
	    list_int *listPK_Attribute_System_Configuration = pMediaFile->m_mapPK_Attribute_Find(ATTRIBUTETYPE_System_Configuration_CONST);
	    int PK_Attribute_System_Configuration = listPK_Attribute_System_Configuration && 
	      listPK_Attribute_System_Configuration->size() ? *(listPK_Attribute_System_Configuration->begin()) : 0;
	    sSystemConfiguration = m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeName(PK_Attribute_System_Configuration);
 	  }

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Game_PlugIn::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());
 
	string mediaURL = (sSystemConfiguration.empty() ? sFileToPlay + "~" + sSystemConfiguration : sFileToPlay);
	string Response;
 
	mediaURL = sFileToPlay;
 
	// send the CMD straight through.
 
	// Fill out the stream bits for OSD
	string sROMName = FileUtils::FileWithoutExtension(FileUtils::FilenameWithoutPath(mediaURL));

	if (m_pGAMEROM->getTitleForROM(sROMName) != "NOMATCH")
	  {
	    pGameMediaStream->m_bPlugInWillSetDescription=true;
	    pGameMediaStream->m_sMediaDescription = m_pGAMEROM->getTitleForROM(sROMName);
	    if (m_pGAMEROM->getSubTitleForROM(sROMName) != "NOMATCH" ||
		m_pGAMEROM->getSubTitleForROM(sROMName).size() > 2)
	      {
		pGameMediaStream->m_sMediaDescription += " \n("+m_pGAMEROM->getSubTitleForROM(sROMName)+")";
	      }
	  }
	if (m_pGAMEROM->getManufacturerForROM(sROMName) != "NOMATCH")
	  {
	    pGameMediaStream->m_sSectionDescription = m_pGAMEROM->getManufacturerForROM(sROMName);
	    if (m_pGAMEROM->getYearForROM(sROMName) != "NOMATCH" ||
		!m_pGAMEROM->getYearForROM(sROMName).empty())
	      {
		pGameMediaStream->m_sSectionDescription += " \n("+m_pGAMEROM->getYearForROM(sROMName)+")";
	      }
	  }

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
		pGameMediaStream->m_sAppName = "stella.stella";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_a2600_CONST;
	}

	if (mediaURL.find("/a5200") != string::npos || StringUtils::ToLower(mediaURL).find(".a52") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_a5200_CONST;
		pGameMediaStream->m_sKeypadOverlayPath = "/home/snap/a5200/keypads/";
	}

	if (mediaURL.find("/a7800") != string::npos || StringUtils::ToLower(mediaURL).find(".a78") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_a7800_CONST;
	}

	if (mediaURL.find("/coleco") != string::npos || StringUtils::ToLower(mediaURL).find(".col") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_coleco_CONST;
	}

	if (mediaURL.find("/intv") != string::npos || StringUtils::ToLower(mediaURL).find(".int") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_intv_CONST;
		pGameMediaStream->m_sKeypadOverlayPath = "/home/snap/intv/keypads/";
	}

	if (mediaURL.find("/sg1000") != string::npos || StringUtils::ToLower(mediaURL).find(".sg") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_sg1000_CONST;
	}

	if (mediaURL.find("/sms") != string::npos || StringUtils::ToLower(mediaURL).find(".sms") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_sms_CONST;
	}

	if (mediaURL.find("/nes") != string::npos || StringUtils::ToLower(mediaURL).find(".nes") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_nes_CONST;
	}

	if (mediaURL.find("/famicom") != string::npos || StringUtils::ToLower(mediaURL).find(".fam") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_famicom_CONST;
	}

	if (mediaURL.find("/snespal") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_snespal_CONST;
	}
        if (mediaURL.find("/snes") != string::npos || StringUtils::ToLower(mediaURL).find(".smc") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".sfc") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".fig") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".swc") != string::npos)
        {
                pGameMediaStream->m_sAppName = "mame.mame";
                pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_snes_CONST; 
        }

	if (mediaURL.find("/genesis") != string::npos || 
		StringUtils::ToLower(mediaURL).find(".smd") != string::npos || 
		StringUtils::ToLower(mediaURL).find(".md") != string::npos ||
		StringUtils::ToLower(mediaURL).find(".gen") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_genesis_CONST; 
	}

	if (mediaURL.find("/tg16") != string::npos 
		&& StringUtils::ToLower(mediaURL).find(".pce") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_tg16_CONST;
	}

        if (mediaURL.find("/pce") != string::npos
                && StringUtils::ToLower(mediaURL).find(".pce") != string::npos)
        {
                pGameMediaStream->m_sAppName = "mame.mame";
                pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_pce_CONST;
        }

        if (mediaURL.find("/sgx") != string::npos
                && StringUtils::ToLower(mediaURL).find(".pce") != string::npos)
        {
                pGameMediaStream->m_sAppName = "mame.mame";
                pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_sgx_CONST;
        }

	if (mediaURL.find("/vectrex") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".vec") != string::npos 
		|| StringUtils::ToLower(mediaURL).find(".gam") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_vectrex_CONST;
	}

        if (mediaURL.find("/apple2") != string::npos
                || StringUtils::ToLower(mediaURL).find(".dsk") != string::npos
                || StringUtils::ToLower(mediaURL).find(".nib") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".po") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".do") != string::npos)
        {
                pGameMediaStream->m_sAppName = "mame.mame";
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

	if (mediaURL.find("/ti994a") != string::npos
	    || StringUtils::ToLower(mediaURL).find(".rpk") != string::npos)
	{
	  pGameMediaStream->m_sAppName = "mame.mame";
	  pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_TI99_CONST;
	}

	if (mediaURL.find("/a800") != string::npos
	    || mediaURL.find("/a130xe") != string::npos
	    || StringUtils::ToLower(mediaURL).find(".atr") != string::npos
	    || StringUtils::ToLower(mediaURL).find(".xfd") != string::npos
	    || StringUtils::ToLower(mediaURL).find(".rom") != string::npos
	    || StringUtils::ToLower(mediaURL).find(".xex") != string::npos)
	  {
	    pGameMediaStream->m_sAppName = "atari800.atari800";
	    pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_Atari800_CONST;
	  }

	if (mediaURL.find(".j64") != string::npos)
	{
		pGameMediaStream->m_sAppName = "mame.mame";
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

	if (mediaURL.find("/amiga") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".adf") != string::npos
		|| StringUtils::ToLower(mediaURL).find(".ipf") != string::npos)
	{
		pGameMediaStream->m_sAppName = "fs-emu.fs-emu";
		pGameMediaStream->m_iPK_MediaType = MEDIATYPE_lmce_Game_amiga_CONST;
	}
	
	if (pGameMediaStream->StreamingRequired())
	  {

	    // Translate window name for networked versions of emulators.
	    if (pGameMediaStream->m_sAppName == "mame.mame")
	      {
		pGameMediaStream->m_sAppName = "csmame.csmame";
	      }
	    else if (pGameMediaStream->m_sAppName == "mame.mame")
	      {
		pGameMediaStream->m_sAppName = "csmess.csmess";
	      }
	    else if (pGameMediaStream->m_sAppName == "ume.ume")
	      {
		pGameMediaStream->m_sAppName = "csume.csume";
	      }

	    DCE::CMD_Start_Streaming scmd(m_dwPK_Device,
					  pGameMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					  pGameMediaStream->m_iPK_MediaType,
					  pGameMediaStream->m_iStreamID_get(),
					  pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pGameMediaStream->m_sStartPosition,
					  mediaURL,
					  pGameMediaStream->GetTargets(DEVICETEMPLATE_Game_Player_CONST));
	    SendCommand(scmd);
	  }
	else
	  {
	    DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,
					       pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					       pGameMediaStream->m_iPK_MediaType,
					       pGameMediaStream->m_iStreamID_get( ),
					       pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pGameMediaStream->m_sStartPosition,
					       mediaURL);
	    
	    SendCommand(CMD_Play_Media);
	  }

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

/**
 * Playback Completed Handler. Used for handling emulator crashes.
 */
bool Game_PlugIn::PlaybackCompleted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
  PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXX MY SCROTUM !!!!! ");
  /** Confirm this is from one of ours */
  if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate != DEVICETEMPLATE_Game_Player_CONST )
    return false; 

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ONE OF US! ONE OF US! ONE OF US! ONE OF US!");

  /** Get Event Parameters */
  int iStreamID = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str());
  string sURL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
  bool bWithErrors = pMessage->m_mapParameters[EVENTPARAMETER_With_Errors_CONST]=="1";
  string sDeviceList = "";

  /** Find the stream */
  GameMediaStream *pGameMediaStream = NULL;
  MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find(iStreamID,pMessage->m_dwPK_Device_From);

  if (!pMediaStream || (pGameMediaStream = ConvertToGameMediaStream(pMediaStream,"Game_Plugin::PlaybackCompleted(): ")) == NULL)
    return false;

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AGAIN ONE OF US! AGAIN ONE OF US! AGAIN ONE OF US!");

  /** We're going to send a message to all the orbiters in this area so they know what the remote is,
      and we will pop up a message on their screen. */


  // Don't fire the message if there are no errors. 
  if (!bWithErrors)
    return false;

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WE HAZ UN ERRUR!");

  for( MapEntertainArea::iterator itEA = pGameMediaStream->m_mapEntertainArea.begin( );itEA != pGameMediaStream->m_mapEntertainArea.end( );++itEA )
    {
      EntertainArea *pEntertainArea = ( *itEA ).second;
      LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
      for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
	  OH_Orbiter *pOH_Orbiter = (*it).second;
	  if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
	    continue;
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
	  sDeviceList += StringUtils::itos(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device) + ",";
	}
    }

  sDeviceList = sDeviceList.substr(0,sDeviceList.size()-1); // drop comma.

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sDeviceList %s",sDeviceList.c_str());

  string sText = "I am sorry, the Game you were playing has unexpectedly crashed. If this persists, please try a different game.";
  string sOptions = "Ok|";
  CMD_Display_Dialog_Box_On_Orbiter_Cat db(m_dwPK_Device,DEVICECATEGORY_Orbiter_Plugins_CONST,
					   false, BL_SameHouse,
					   sText,
					   sOptions,
					   sDeviceList); // FIXME: make proper list.
  SendCommandNoResponse(db);
  
  // Do not discard this event, let it continue bubbling through the interceptors.
  return false;
  
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
//<-dceag-c1119-b->

	/** @brief COMMAND: #1119 - Bind Keypad */
	/** Used on Remotes that have keypads to update the Keypad overlay, if available. */
		/** @param #3 PK_DesignObj */
			/** The designobj of the keypad overlay. */
		/** @param #45 PK_EntertainArea */
			/** The Entertainment area of the stream.  */

void Game_PlugIn::CMD_Bind_Keypad(string sPK_DesignObj,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c1119-e->
{
  if (!m_pMedia_Plugin)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_PlugIn::CMD_Bind_Keypad - Could not find media plugin. Bailing!");
    }

  OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
  vector<EntertainArea *> vectEntertainArea;
  m_pMedia_Plugin->DetermineEntArea(pMessage->m_dwPK_Device_From, 0, sPK_EntertainArea, vectEntertainArea);
  
  if (vectEntertainArea.size()!= 1 || !pOH_Orbiter)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_PlugIn::CMD_Bind_Keypad - Could not determine EA.");
      sCMD_Result="Can't find ent area/orbiter";
      return;
    }

  EntertainArea *pEntertainArea = vectEntertainArea[0];

  if (!pEntertainArea->m_pMediaStream)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_PlugIn::CMD_Bind_Keypad - Could not determine media stream");
      sCMD_Result="No media stream";
      return;
    }
  
  GameMediaStream *pGameMediaStream = ConvertToGameMediaStream(pEntertainArea->m_pMediaStream, "Game_Plugin::Bind_Keypad(): ");
  if (!pGameMediaStream)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_PlugIn::CMD_Bind_Keypad - Could not get GameMediaStream.");
      sCMD_Result="Could not derive Game Media Stream";
      return;
    }

  MediaFile *pMediaFile = pGameMediaStream->GetCurrentMediaFile();

  if (!pMediaFile)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_PlugIn::CMD_Bind_Keypad - Could not retrieve the MediaFile.");
      sCMD_Result="No media file";
      return;
    }

  string sKeypadPath = pGameMediaStream->m_sKeypadOverlayPath + "/" + pMediaFile->m_sTitle + ".png";
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_PlugIn::CMD_Bind_Keypad - sKeypadPath is %s",sKeypadPath.c_str());
      
  if (FileUtils::FileExists(sKeypadPath))
    {
      size_t iData_Size;
      char *pData = FileUtils::ReadFileIntoBuffer(sKeypadPath,iData_Size);
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_PlugIn::CMD_Bind_Keypad - Updating Keypad overlay designobj %s with png image size %d",sPK_DesignObj.c_str(),iData_Size);
      DCE::CMD_Update_Object_Image Update_Object_Image(m_dwPK_Device,pMessage->m_dwPK_Device_From,sPK_DesignObj,"3",pData,iData_Size,"1");
      SendCommand(Update_Object_Image);
      delete pData;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_PlugIn::CMD_Bind_Keypad - Could not find a keypad, path %s.",sKeypadPath.c_str());
    }

}
//<-dceag-c764-b->

	/** @brief COMMAND: #764 - Set Active Menu */
	/** Used to set which screens to show given monitor mode, options, etc. */
		/** @param #9 Text */
			/** The menu currently active */

void Game_PlugIn::CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c764-e->
{
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Game_PlugIn::CMD_Set_Active_Menu %s",sText.c_str());

  GameMediaStream *pGameMediaStream = 
    ConvertToGameMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[pMessage->m_dwPK_Device_From],pMessage->m_dwPK_Device_From));

  if (!pGameMediaStream)
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Game_PlugIn::CMD_Set_Active_Menu stream is NULL");
      return;
    }

  int PK_Screen_Remote_Alt;

  // COMEBACKHERE

}
