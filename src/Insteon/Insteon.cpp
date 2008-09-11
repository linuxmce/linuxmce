/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648

     Copyright (C) 2007 Peter Kalogiannis, <pkalogiannis@gmail.com>

     This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the Free
     Software Foundation; either version 2 of the License, or (at your option)
     any later version.

     This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of 
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
//<-dceag-d-b->
#include "Insteon.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "InsteonJobSwitchChangeLevel.h"
#include "InsteonJobReset.h"
#include "InsteonJobEZRainValveControl.h"

#define POOL_DELAY     200000
bool Insteon::m_AsynchStarted = false;
/*bool ZWave::m_PoolStarted = false;
bool ZWave::m_ZWaveChanged = false;
bool ZWave::m_ReceiveStarted = false;
bool ZWave::m_ReportChildrenStarted = false;
*/

pthread_t Insteon::m_AsynchThread = (pthread_t)NULL;
/*pthread_t Insteon::m_PoolThread = (pthread_t)NULL;
pthread_t Insteon::m_ReceiveThread = (pthread_t)NULL;
*/

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Insteon::Insteon(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Insteon_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_InsteonMutex("insteon")
	, m_PoolingMutex("insteon_pooling")
	, m_InsteonAPI(NULL)
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_InsteonMutex.Init( &m_MutexAttr );
  
	m_InsteonAPI = LMCEInsteonAPI::instance();
	if( m_InsteonAPI == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Not enough memory for Insteon API");
	}
	m_InsteonAPI->setLMCE(this);
	
}
/*
//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Insteon::Insteon(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Insteon_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}
*/


//<-dceag-dest-b->
Insteon::~Insteon()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Insteon::GetConfig()
{
	if( !Insteon_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Insteon::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->!
/*Insteon_Command *Create_Insteon(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Insteon(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}*/
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
void Insteon::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	
/*	PLUTO_SAFETY_LOCK(im,m_InsteonMutex); */
	sCMD_Result = "UNHANDLED CHILD";

	const char* NodeID = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str();
	{
		sCMD_Result = "OK";
		if( pMessage->m_dwID == COMMAND_Generic_On_CONST )
		{
		//	LoggerWrapper::GetInstance()->Write(LV_INSTEON, "Sending ON - %s", NodeID);
			InsteonJobSwitchChangeLevel * light = new InsteonJobSwitchChangeLevel(m_InsteonAPI, 0xFF, (char*)NodeID);
			if( light != NULL )
			{
				//light->setRunningTimeout( ZW_SHORT_TIMEOUT );
				m_InsteonAPI->insertJob( light );
				m_InsteonAPI->start();
			}
		}
		if( pMessage->m_dwID == COMMAND_Generic_Off_CONST )
		{
		//	LoggerWrapper::GetInstance()->Write(LV_INSTEON, "Sending OFF - %s", NodeID);
			InsteonJobSwitchChangeLevel * light = new InsteonJobSwitchChangeLevel(m_InsteonAPI, 0, (char*)NodeID);
			if( light != NULL )
			{
				//light->setRunningTimeout( ZW_SHORT_TIMEOUT );
				m_InsteonAPI->insertJob( light );
				m_InsteonAPI->start();
			}
		}
		else if( pMessage->m_dwID == COMMAND_Set_Level_CONST )
		{
			int req_level = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
//			LoggerWrapper::GetInstance()->Write(LV_Insteon, "Sending LEVEL - %d || %d", level, NodeID);
			int level = ((float)req_level) * 2.55;
			InsteonJobSwitchChangeLevel * light = new InsteonJobSwitchChangeLevel(m_InsteonAPI, req_level > 99 ? 0xFF : level, (char*)NodeID);
			if( light != NULL )
			{
				m_InsteonAPI->insertJob( light );
				m_InsteonAPI->start();
			}
		}
		else if( pMessage->m_dwID == COMMAND_TurnOnSprinklerValve_CONST)
		{
			unsigned char ValveNum = (unsigned char)atoi(pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Valve_Number_int_CONST).c_str());
			InsteonJobEZRainValveControl* valve_control = new InsteonJobEZRainValveControl(m_InsteonAPI, ValveNum, 0x1, (char*)NodeID);
			if( valve_control != NULL )
			{
				m_InsteonAPI->insertJob(valve_control );
				m_InsteonAPI->start();
			}
		}
		else if( pMessage->m_dwID == COMMAND_TurnOffSprinklerValve_CONST)
		{
			unsigned char ValveNum = (unsigned char)atoi(pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Valve_Number_int_CONST).c_str());
			InsteonJobEZRainValveControl* valve_control = new InsteonJobEZRainValveControl(m_InsteonAPI,ValveNum, 0, (char*)NodeID);
			if( valve_control != NULL )
			{
				m_InsteonAPI->insertJob(valve_control );
				m_InsteonAPI->start();
			}
		}			
	}
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Insteon::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

bool Insteon::ConfirmConnection()
{
	PLUTO_SAFETY_LOCK(insm,m_InsteonMutex);
	
	if( m_InsteonAPI != NULL )
	{
		if(!m_InsteonAPI->isInitialized() )
		{
			if( !m_InsteonAPI->GetPLCHandle())
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Insteon::ConfirmConnection : Cannot get handle");
			}
		}
		else
		{
			// start ZWave pooling
		//	CMD_Pool(true);
			return true;
		}
	}
	else
	{
	//	CMD_Pool(false);
		asynchThread(false);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Insteon::ConfirmConnection : error");
	}
	
	return false;
}

void *DoAsynch(void *p)
{
	Insteon *pInsteon = (Insteon *) p;
	if( pInsteon != NULL )
	{
		pInsteon->AsynchInsteon();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "DoAsynch : no Insteon object");
	}
	
	return NULL;
}

void Insteon::AsynchInsteon()
{
	while(m_AsynchStarted)
	{
		m_InsteonAPI->listenAsynchronous();
		usleep(POOL_DELAY); 
	}
}


void Insteon::asynchThread(bool start)
{
#ifndef NO_ASYNCH
	if( start )
	{
		if( !m_AsynchStarted)
		{
			if( !pthread_create(&m_AsynchThread, NULL, DoAsynch, (void*)this) )
			{
				m_AsynchStarted = true;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"asynchThread : couldn't create the thread");
			}
		}
	}
	else
	{
		if( m_AsynchStarted )
		{
//			m_InsteonAPI->stop();
//			m_InsteonAPI->clearJobs();
			m_AsynchStarted = false;
			pthread_join(m_AsynchThread, NULL);
		}
/*		if( m_ReceiveStarted )
		{
			pthread_join(m_ReceiveThread, NULL);
		}
*/
	}
#endif
}




//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Insteon::SomeFunction()
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

//<-dceag-c756-b->

	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void Insteon::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
	cout << "Need to implement command #756 - Report Child Devices" << endl;
}

//<-dceag-c757-b->

	/** @brief COMMAND: #757 - Download Configuration */
	/** Download new configuration data for this device */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

//<-dceag-c760-b->

	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

void Insteon::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
//<-dceag-c760-e->
{
	const char* NodeID = sID.c_str();
	sCMD_Result = "OK";
	if( iPK_Command == COMMAND_Generic_On_CONST )
	{
		//LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Sending ON - %s", sID.c_str());
		InsteonJobSwitchChangeLevel * light = new InsteonJobSwitchChangeLevel(m_InsteonAPI, 0xFF, (char*)NodeID);
		if( light != NULL )
		{
			//light->setRunningTimeout( ZW_SHORT_TIMEOUT );
			m_InsteonAPI->insertJob( light );
			m_InsteonAPI->start();
		}
	}
	
	else if( iPK_Command == COMMAND_Generic_Off_CONST )
	{
		//LoggerWrapper::GetInstance()->Write(LV_INSTEON,"Sending OFF - %s", sID.c_str());
		InsteonJobSwitchChangeLevel * light = new InsteonJobSwitchChangeLevel(m_InsteonAPI, 0x00, (char*)NodeID);
		if( light != NULL )
		{
			//light->setRunningTimeout( ZW_SHORT_TIMEOUT );
			m_InsteonAPI->insertJob( light );
			m_InsteonAPI->start();
		}
	}
	
	else 
	{
	cout << "Unhandled Command in \"Send Command to Child\" command for node: " << NodeID << endl;	
	}


}

//<-dceag-c776-b->

	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

void Insteon::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c776-e->
{
	InsteonJobReset* reset_job = new InsteonJobReset(m_InsteonAPI);
	m_InsteonAPI->insertJob( reset_job );
	m_InsteonAPI->start();
}

//<-dceag-c788-b->

	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

void Insteon::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
{
	cout << "Need to implement command #788 - StatusReport" << endl;
	cout << "Parm #51 - Arguments=" << sArguments << endl;
}


//<-dceag-c920-b->

	/** @brief COMMAND: #920 - Add Insteon Device to Group */
	/** Create a group and add the specified devices to the group number (stored in PLC adapter) */
		/** @param #249 Group ID */
			/** The Insteon Group to which to add the node */
		/** @param #250 Nodes List */
			/** The list of associated Z-Wave nodes (comma separated) */

void Insteon::CMD_Add_Insteon_Device_to_Group(int iGroup_ID,string sNodes_List,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
	
}

