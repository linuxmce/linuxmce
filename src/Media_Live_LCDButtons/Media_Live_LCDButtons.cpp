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
#include "Media_Live_LCDButtons.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#ifdef WIN32
	void* KeyboardLoop(void* param) { return NULL; }
	int StartLCD() { return 0; }
	void (*g_ButtonCallBackFn)(int PK_Button);
	int g_QuitButtonThread;
#else
	#include "led.h"
	#include "libvfd.h"
	extern void (*g_ButtonCallBackFn)(int PK_Button);
	extern int g_QuitButtonThread;
#endif

#include "pluto_main/Define_MediaType.h"

#define	MAX_CHARS_PER_LINE	18
#define MAX_CHARS_SCROLLABLE	40
#define NUMLINES		2

Media_Live_LCDButtons *g_pMedia_Live_LCDButtons = NULL;


void ButtonCallBackFn(int PK_Button)
{
	if( g_pMedia_Live_LCDButtons )
		g_pMedia_Live_LCDButtons->ButtonCallBack(PK_Button);
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Media_Live_LCDButtons::Media_Live_LCDButtons(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Media_Live_LCDButtons_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, VFD_LCD_Base(MAX_CHARS_SCROLLABLE,NUMLINES,MAX_CHARS_PER_LINE)
{
	m_pDevice_Orbiter=NULL;
	m_KeyboardLoopThread_Id=0;
	m_VfdHandle=0;
	m_iVfdVolumeLevel_Last=m_iVfdScrollRegionLast=m_iSourceIcon_Last=m_iPlayBackIcon_Last=0;
	m_bSpeakerIcon_Last=m_bVolumeIcon_Last=false;
	g_pMedia_Live_LCDButtons=this;
	g_ButtonCallBackFn=ButtonCallBackFn;
	m_pKeyboardDevice=NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Media_Live_LCDButtons::~Media_Live_LCDButtons()
//<-dceag-dest-e->
{
	g_QuitButtonThread=true;
	g_pMedia_Live_LCDButtons=NULL;
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Joining Keyboard Loop Thread" );
	if( m_KeyboardLoopThread_Id )
	{
		pthread_cancel(m_KeyboardLoopThread_Id);
		pthread_join(m_KeyboardLoopThread_Id,NULL);
	}
	delete m_pKeyboardDevice;
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Done" );
}

//<-dceag-getconfig-b->
bool Media_Live_LCDButtons::GetConfig()
{
	if( !Media_Live_LCDButtons_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDevice_Orbiter = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Standard_Orbiter_CONST);
	if( m_pDevice_Orbiter )
	{
		string sBlockDevice = DATA_Get_Block_Device();
		if( sBlockDevice.empty() )
			sBlockDevice = "/dev/input/event0";
		m_pKeyboardDevice = strdup( sBlockDevice.c_str() );
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::GetConfig Orbiter: %d dev: %s", m_pDevice_Orbiter->m_dwPK_Device, m_pKeyboardDevice);
		if(pthread_create( &m_KeyboardLoopThread_Id, NULL, KeyboardLoop, (void*) m_pKeyboardDevice) )
		{
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create PNP thread" );
		}
	}

	m_VfdHandle=StartLCD();

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Media_Live_LCDButtons::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Media_Live_LCDButtons::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Media_Live_LCDButtons::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-c406-b->

	/** @brief COMMAND: #406 - Display Message */
	/** Display a message on the lcd/vfd display */
		/** @param #9 Text */
			/** The message to display */
		/** @param #14 Type */
			/** For devices implementing VFD_LCD_Base, this is the message type defined in the header */
		/** @param #50 Name */
			/** you can give the message a name, such as "status", "error", etc */
		/** @param #102 Time */
			/** Number of seconds to display the message for */
		/** @param #103 List PK Device */
			/** If going to a plugin that wil relay messages to other devices (ie orbiter_plugin and orbiter), A comma delimited list of devices to display this message on.  If going to a display device directly (like vfd/lcd) this is ignored. */

void Media_Live_LCDButtons::CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c406-e->
{
	if( !m_VfdHandle )
		return;
	NewMessage(atoi(sType.c_str()),sName,sText,atoi(sTime.c_str()));
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::CMD_Display_Message type %d name: %s text: %s time: %d",
		atoi(sType.c_str()),sName.c_str(),sText.c_str(),atoi(sTime.c_str()));
#endif
}

//<-dceag-c837-b->

	/** @brief COMMAND: #837 - Show Media Playback State */
	/** Show the current state of the media playback */
		/** @param #5 Value To Assign */
			/** Empty = no media playing, otherwise a speed, 0=pause, 1000=normal forward, -4000 = 4x reverse, etc. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #76 Level */
			/** The level of the volume, from 0-100.  empty means it's not known, or "MUTE" */

void Media_Live_LCDButtons::CMD_Show_Media_Playback_State(string sValue_To_Assign,int iPK_MediaType,string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c837-e->
{
#ifndef WIN32
	if( !m_VfdHandle )
		return;
	
	int iVfdVolumeLevel_Last=m_iVfdVolumeLevel_Last;
	bool bSpeakerIcon_Last=m_bSpeakerIcon_Last, bVolumeIcon_Last=m_bVolumeIcon_Last;

	if( sLevel!="UNKNOWN" )
	{
		if( sLevel.empty() )
		{
			bSpeakerIcon_Last=bVolumeIcon_Last=false;
		}
		else if( sLevel=="MUTE" )
			bSpeakerIcon_Last=true;
		else
		{
			bSpeakerIcon_Last=false;
			bVolumeIcon_Last=true;
			iVfdVolumeLevel_Last=atoi(sLevel.c_str())/8;
		}

		if( bSpeakerIcon_Last!=m_bSpeakerIcon_Last )
		{
			m_bSpeakerIcon_Last=bSpeakerIcon_Last;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::CMD_Show_Media_Playback_State speaker icon to: %d", (int) m_bSpeakerIcon_Last);
#endif
			if( m_bSpeakerIcon_Last )
				VFDIconOn(m_VfdHandle, VFD_ICON_SPEAKER);
			else
				VFDIconOff(m_VfdHandle, VFD_ICON_SPEAKER);
		}

		if( bVolumeIcon_Last!=m_bVolumeIcon_Last )
		{
			m_bVolumeIcon_Last=bVolumeIcon_Last;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::CMD_Show_Media_Playback_State Volume icon to: %d", (int) m_bVolumeIcon_Last);
#endif
			if( m_bVolumeIcon_Last )
				VFDIconOn(m_VfdHandle, VFD_ICON_VOLUME);
			else
				VFDIconOff(m_VfdHandle, VFD_ICON_VOLUME);
		}

		if( iVfdVolumeLevel_Last!=m_iVfdVolumeLevel_Last )
		{
			m_iVfdVolumeLevel_Last=iVfdVolumeLevel_Last;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::CMD_Show_Media_Playback_State Volume level: %d", (int) m_bVolumeIcon_Last);
#endif
			VFDSetVolume(m_VfdHandle, m_iVfdVolumeLevel_Last);
		}
	}

	int PlayBackIcon=m_iPlayBackIcon_Last;
	if( sValue_To_Assign.empty()==false )
	{
		int iSpeed=atoi( sValue_To_Assign.c_str() );
		if( iSpeed==0 )
			PlayBackIcon=VFD_ICON_PAUSE;
		else if( iSpeed>0 && iSpeed<=1000 )
			PlayBackIcon=VFD_ICON_PLAY;
		else if( iSpeed>1000 )
			PlayBackIcon=VFD_ICON_FFWD;
		else if( iSpeed<0 )
			PlayBackIcon=VFD_ICON_RWND;
	}
	else
		PlayBackIcon=0;
	
	int iSourceIcon_Last=m_iSourceIcon_Last;
	switch( iPK_MediaType )
	{
		case MEDIATYPE_pluto_DVD_CONST:
		case MEDIATYPE_np_DVD_CONST:
			iSourceIcon_Last=VFD_ICON_DVD;
			break;

		case MEDIATYPE_pluto_LiveTV_CONST:
		case MEDIATYPE_pluto_StoredVideo_CONST:
			iSourceIcon_Last=VFD_ICON_VIDEO;
			break;
			
		case MEDIATYPE_pluto_CD_CONST:
		case MEDIATYPE_pluto_StoredAudio_CONST:
			iSourceIcon_Last=VFD_ICON_CD;
			break;
	
		case MEDIATYPE_pluto_LiveRadio_CONST:
		case MEDIATYPE_np_OTARadio_CONST:
		case MEDIATYPE_np_SatelliteRadio_CONST:
		case MEDIATYPE_np_CableRadio_CONST:
			iSourceIcon_Last=VFD_ICON_RADIO;
			break;	
		default:
			iSourceIcon_Last=0;
	}
	if( PlayBackIcon==m_iPlayBackIcon_Last && m_iSourceIcon_Last==iSourceIcon_Last )
		return; // Nothing to do.  The state is unchanged

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::CMD_Show_Media_Playback_State Changing playback state %s from %d to %d source %d from %d to %d",
		sValue_To_Assign.c_str(),m_iPlayBackIcon_Last,PlayBackIcon,iPK_MediaType,m_iSourceIcon_Last,iSourceIcon_Last);
#endif

	if( m_iPlayBackIcon_Last!=PlayBackIcon )
	{
		if( m_iPlayBackIcon_Last )
			VFDIconOff(m_VfdHandle, m_iPlayBackIcon_Last);
		m_iPlayBackIcon_Last=PlayBackIcon;
		if( m_iPlayBackIcon_Last )
			VFDIconOn(m_VfdHandle, m_iPlayBackIcon_Last);
	}


	if( m_iSourceIcon_Last!=iSourceIcon_Last )
	{
		if( m_iSourceIcon_Last )
			VFDIconOff(m_VfdHandle, m_iSourceIcon_Last);
		m_iSourceIcon_Last=iSourceIcon_Last;
		if( m_iSourceIcon_Last )
			VFDIconOn(m_VfdHandle, m_iSourceIcon_Last);
	}
#endif
}

void Media_Live_LCDButtons::DoUpdateDisplay(vector<string> *vectString)
{
#ifndef WIN32
	size_t size1=0,size2=0;  // The length of each line
	string sLine1,sLine2;
	if( vectString->size()>=1 )
	{
		sLine1 = (*vectString)[0];

		if( sLine1!=m_sLine1Last )
		{
			VFDSetString(m_VfdHandle, VFD_STR_REGION_1, 0, (unsigned char *) sLine1.c_str());
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::DoUpdateDisplay line 1 %d=%s", size1, sLine1.c_str());
#endif
			m_sLine1Last=sLine1;
		}
	}
	if( vectString->size()>=2 )
	{
		sLine2 = (*vectString)[1];

		if( sLine2!=m_sLine2Last )
		{
			VFDSetString(m_VfdHandle, VFD_STR_REGION_3, 0, (unsigned char *) sLine2.c_str());
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::DoUpdateDisplay line 2 %d=%s", size2, sLine2.c_str());
#endif
			m_sLine2Last = sLine2;
		}
	}

	int iVfdScrollRegionLast;

	size1 = sLine1.size();
	size2 = sLine2.size();

	if( size1<=MAX_CHARS_PER_LINE && size2<=MAX_CHARS_PER_LINE )
		iVfdScrollRegionLast = 0;
	else if( size1<=MAX_CHARS_PER_LINE )
		iVfdScrollRegionLast = VFD_SCROLL_REGION3;
	else
		iVfdScrollRegionLast = VFD_SCROLL_REGION1;

	if( iVfdScrollRegionLast!=m_iVfdScrollRegionLast )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::DoUpdateDisplay m_iVfdScrollRegionLast %d iVfdScrollRegionLast %d",m_iVfdScrollRegionLast, iVfdScrollRegionLast);
#endif
		m_iVfdScrollRegionLast = iVfdScrollRegionLast;
		VFDSetScrollRegion( m_VfdHandle, m_iVfdScrollRegionLast );
	}
#endif
}

void Media_Live_LCDButtons::ButtonCallBack(int PK_Button)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Live_LCDButtons::ButtonCallBackFn Button %d",PK_Button);
	DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress(m_dwPK_Device,m_pDevice_Orbiter->m_dwPK_Device,StringUtils::itos(PK_Button),0,"keypad");
	SendCommand(CMD_Simulate_Keypress);
}

