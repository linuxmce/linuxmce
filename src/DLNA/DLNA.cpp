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

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Installation.h"

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

	LoadEntertainAreas();

	m_pEngine = new DLNAEngine(this);
	m_pEngine->Init();

	PurgeInterceptors();

	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Start_Streaming_CONST );
	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Play_Media_CONST );
	RegisterMsgInterceptor(( MessageInterceptorFn )( &DLNA::MediaCommandIntercepted ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Completed_CONST );

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

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void DLNA::SomeFunction()
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

bool DLNA::LoadEntertainAreas()
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "DLNA::LoadEntertainAreas() start %d",  m_pData->m_dwPK_Installation );

	Database_pluto_main* pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
// TODO: don't hardcode
	string host = "localhost", dbuser = "root", dbpass = "", dbname = "pluto_main";
	int dbport = 3306;
	if( !pDatabase_pluto_main->Connect( host, dbuser, dbpass, dbname, dbport ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
		OnQuit();
		return false;
	}

	Row_Installation *pRow_Installation = pDatabase_pluto_main->Installation_get( )->GetRow( m_pData->m_dwPK_Installation );
	vector<Row_Room *> vectRow_Room; // Ent Areas are specified by room. Get all the rooms first
	pRow_Installation->Room_FK_Installation_getrows( &vectRow_Room );
	for( size_t iRoom=0;iRoom<vectRow_Room.size( );++iRoom )
	{
		Row_Room *pRow_Room=vectRow_Room[iRoom];
		vector<Row_EntertainArea *> vectRow_EntertainArea;
		pRow_Room->EntertainArea_FK_Room_getrows( &vectRow_EntertainArea );
		for( size_t s=0;s<vectRow_EntertainArea.size( );++s )
		{
			Row_EntertainArea *pRow_EntertainArea = vectRow_EntertainArea[s];
			EntertainArea *pEntertainArea = new EntertainArea( pRow_EntertainArea->PK_EntertainArea_get( ),
									   pRow_EntertainArea->Description_get() );
			m_mapEntertainAreas[pEntertainArea->m_iPK_EntertainArea]=pEntertainArea;
			// Now find all the devices in the ent area
			vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
			pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows( &vectRow_Device_EntertainArea );
			for( size_t s2=0;s2<vectRow_Device_EntertainArea.size( );++s2 )
			{
				Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s2];
				if( !pRow_Device_EntertainArea || !pRow_Device_EntertainArea->FK_Device_getrow( ) )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device_EntertainArea refers to a NULL device %d %d",pRow_Device_EntertainArea->FK_EntertainArea_get(),pRow_Device_EntertainArea->FK_Device_get());
					continue;
				}
				LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DLNA::LoadEntertainAreas() assigning PK_Device %d to EA %d", pRow_Device_EntertainArea->FK_Device_get(), pEntertainArea->m_iPK_EntertainArea);
				pEntertainArea->m_setDevices.insert(pRow_Device_EntertainArea->FK_Device_get());
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
	if ((pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND && pMessage->m_dwID == COMMAND_Start_Streaming_CONST))
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
	} else if (pMessage->m_dwMessage_Type == MESSAGETYPE_EVENT && pMessage->m_dwID == EVENT_Playback_Completed_CONST) {
		// Event only has StreamID, need to look up renderer with that
		string sStreamID = pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST];
		int iStreamID = atoi(sStreamID.c_str());
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
