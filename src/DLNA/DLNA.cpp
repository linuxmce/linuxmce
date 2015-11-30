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
#include "DLNA.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DLNAEngine.h"
#include "LMCERenderer.h"

#include "pluto_main/Define_EventParameter.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
DLNA::DLNA(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: DLNA_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
DLNA::DLNA(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: DLNA_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
DLNA::~DLNA()
//<-dceag-dest-e->
{
	for(map<int,EntertainArea *>::iterator it=m_mapEntertainAreas.begin();it!=m_mapEntertainAreas.end();++it)
		delete (*it).second;
	delete m_pEngine;
}

//<-dceag-getconfig-b->
bool DLNA::GetConfig()
{
	if( !DLNA_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	m_bEnableMediaRenderer = true; //DATA_Get_();
	m_bEnableMediaServer = false;
    m_bEnableControlPoints = false;

	LoadEntertainAreas();

	m_pEngine = new DLNAEngine(this);
	m_pEngine->Init();

	PurgeInterceptors();

	// We intercept play and stop commands (and events) to update the status of our renderers
        // the start/stop streaming are used in some cases, and play/stop media in other cases, so we need both
	// also, we need to intercept the playback completed event, as this is emited when the device itself are done playing(song over etc.)
	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Start_Streaming_CONST );
	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Stop_Streaming_CONST );
	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Play_Media_CONST );
	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Stop_Media_CONST );
	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Completed_CONST );
	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Media_Position_Changed_CONST );

	return true;
}

void DLNA::OnQuit()
{
	DLNA_Command::OnQuit();
	m_pApp->exit();
}

void DLNA::OnReload()
{
	DLNA_Command::OnQuit();
        m_pApp->exit();
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool DLNA::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
DLNA_Command *Create_DLNA(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new DLNA(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void DLNA::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void DLNA::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

bool DLNA::LoadEntertainAreas()
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "DLNA::LoadEntertainAreas() start %d",  m_pData->m_dwPK_Installation );

    string eaString;
    CMD_Get_Entertainment_Areas_DT cmdEA(m_dwPK_Device, DEVICETEMPLATE_General_Info_Plugin_CONST, BL_SameHouse, &eaString);
    if (!SendCommand(cmdEA)) {
        return false;
    }
    vector<string> vectEAs;
    StringUtils::Tokenize(eaString, "\n", vectEAs);
    for (size_t i = 0; i < vectEAs.size(); i++) {
        vector<string> vectEAData;
        StringUtils::Tokenize(vectEAs[i], "\t", vectEAData);
        if (vectEAData.size() >= 2) {
            long PK_EA = atoi(vectEAData[0].c_str());
            long PK_Room = atoi(vectEAData[1].c_str());
            LoggerWrapper::GetInstance()->Write( LV_STATUS, "DLNA::LoadEntertainAreas() EA %d",  PK_EA );

            EntertainArea *pEntertainArea = new EntertainArea( PK_EA, vectEAData[2] );
            m_mapEntertainAreas[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;
            Map_DeviceData_Base devices = m_pData->m_AllDevices.m_mapDeviceData_Base;
            for(Map_DeviceData_Base::iterator it=devices.begin(); it!=devices.end();++it) {
                DeviceData_Base *pDeviceData_Base = (*it).second;
                if (pDeviceData_Base->m_dwPK_Room == PK_Room) {
                    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::LoadEntertainAreas() assigning PK_Device %d to EA %d", pDeviceData_Base->m_dwPK_Device, PK_EA);
                    pEntertainArea->m_setDevices.insert(pDeviceData_Base->m_dwPK_Device);
                }
            }
        }
    }
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "DLNA::LoadEntertainAreas() end" );
}

map<int, EntertainArea*>* DLNA::GetEntertainAreas()
{
	return &m_mapEntertainAreas;
}

bool DLNA::MediaCommandIntercepted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::MediaCommandIntercepted() start");
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::MediaCommandIntercepted() pMessage->m_dwID = %d", pMessage->m_dwID);
	
        // Find out what PK_Device is playing, and if it is one of ours
	long PK_Device = 0;
	EntertainArea *pEntertainArea = NULL;
	if (pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && 
	    (pMessage->m_dwID == COMMAND_Start_Streaming_CONST || pMessage->m_dwID == COMMAND_Stop_Streaming_CONST))
	{
		// devices like Slim Server Streamer will need to receive commands for its childen (Pause Media command is sent to that one when playing to Squeezeboxes)
		PK_Device = pDeviceTo->m_dwPK_Device;
		vector<string>vectPK_Devices;
		StringUtils::Tokenize(pMessage->m_mapParameters[COMMANDPARAMETER_StreamingTargets_CONST], ",", vectPK_Devices);
		for (int i = 0; i < vectPK_Devices.size() && pEntertainArea == NULL; i++) {
			int iPK_Device = atoi(vectPK_Devices[i].c_str());
			pEntertainArea = FindEntertainAreaForDevice(iPK_Device);
			if (pEntertainArea != NULL)
			{
				LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::MediaCommandIntercepted() StreamingTargets = %d, Pk_EA = %d, pEA = %d",iPK_Device, pEntertainArea->m_iPK_EntertainArea, pEntertainArea);
			}
		}
	} else if ((pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && pMessage->m_dwID == COMMAND_Play_Media_CONST))		
	{
		PK_Device = pDeviceTo->m_dwPK_Device;
		pEntertainArea = FindEntertainAreaForDevice(PK_Device);
	} else if (pMessage->m_dwMessage_Type == MESSAGETYPE_EVENT && 
		   (pMessage->m_dwID == EVENT_Playback_Completed_CONST || pMessage->m_dwID == EVENT_Media_Position_Changed_CONST)) {
		// Event only has StreamID, need to look up renderer with that
		string sStreamID = pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST];
		int iStreamID = atoi(sStreamID.c_str());
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::MediaCommandIntercepted() Playback completed|Media_position_Changed streamID = %d",iStreamID);
		if (iStreamID > 0)
		{
			for(map<int,EntertainArea *>::iterator it=m_mapEntertainAreas.begin();it!=m_mapEntertainAreas.end();++it)
			{
				if ((*it).second->m_pRenderer != NULL && (*it).second->m_pRenderer->GetStreamID() == iStreamID)
				{
					pEntertainArea = (*it).second;
                }
			}
		}
	}

	if (pEntertainArea != NULL && pEntertainArea->m_pRenderer != NULL)
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::MediaCommandIntercepted() PK_EntArea = %d, pRenderer = %d", pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_pRenderer);
		pEntertainArea->m_pRenderer->MediaCommandIntercepted(pMessage, PK_Device);
	} else {
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::MediaCommandIntercepted() Message not handled!!");
	}

	return false;
}

EntertainArea* DLNA::FindEntertainAreaForDevice(int PK_Device)
{
	for (map<int, EntertainArea*>::iterator it = m_mapEntertainAreas.begin(); it != m_mapEntertainAreas.end(); it++)
	{
		EntertainArea *pEA = (*it).second;
		if (pEA != NULL && pEA->m_setDevices.find(PK_Device) != pEA->m_setDevices.end())
		{
//			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::FindEntertainAreaForDevice() PK_Device = %d, PK_EA = %d, pEA = %d", PK_Device, pEA->m_iPK_EntertainArea, pEA);
			return pEA;
		}
	}
	return NULL;
}

EntertainArea* DLNA::FindEntertainArea(int PK_EntertainArea)
{
	
	map<int, EntertainArea*>::iterator it = m_mapEntertainAreas.find(PK_EntertainArea);
	if (it != m_mapEntertainAreas.end())
	{
		return (*it).second;
	}
	return NULL;
}
