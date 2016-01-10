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
#include "MythTV_Backend_Proxy.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#define PROXY_LISTEN_PORT	6543

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MythTV_Backend_Proxy::MythTV_Backend_Proxy(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_Backend_Proxy_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_mutexInternalData("internal-data")
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );

	m_mutexInternalData.Init( &m_MutexAttr );
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
MythTV_Backend_Proxy::MythTV_Backend_Proxy(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: MythTV_Backend_Proxy_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
	, m_mutexInternalData("internal-data")
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );

	m_mutexInternalData.Init( &m_MutexAttr );
}

//<-dceag-dest-b->
MythTV_Backend_Proxy::~MythTV_Backend_Proxy()
//<-dceag-dest-e->
{
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_Backend_Proxy::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will
	get all	commands for your children in ReceivedCommandForChild, where
	pDeviceData_Base is the child device.  If you handle the message, you
	should change the sCMD_Result to OK
*/

bool MythTV_Backend_Proxy::Connect(int iPK_DeviceTemplate ) {
	if(!MythTV_Backend_Proxy_Command::Connect(iPK_DeviceTemplate)) {
		return false;
	}

	DeviceData_Base *pThisDeviceData = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device);
	if ( pThisDeviceData == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "I could not find myself (%d) in the list of devices that i got from the router!", m_dwPK_Device);
		return false;
	}

	proxy_.setHost(pThisDeviceData->GetIPAddress().c_str());
	proxy_.setPort(PROXY_LISTEN_PORT);
	proxy_.setHandler(this);

	proxy_.Run(false);
	return true;
}


//<-dceag-cmdch-b->
void MythTV_Backend_Proxy::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void MythTV_Backend_Proxy::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

void MythTV_Backend_Proxy::ChannelChanged(const char *host, int channelid)
{
	PLUTO_SAFETY_LOCK( mutexData, m_mutexInternalData );

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Client with address %s changed to channel: %d", host, channelid);
	map<string, pair<int, int> >::const_iterator itAdresses;

	if ( (itAdresses = m_mapAddressToDeviceState.find(host)) == m_mapAddressToDeviceState.end() )
		m_mapAddressToDeviceState[host] = make_pair(-1, channelid);
	else
		m_mapAddressToDeviceState[host].second = channelid;

	if ( m_mapAddressToDeviceState[host].first != -1 )
		EVENT_MythTV_Channel_Changed(m_mapAddressToDeviceState[host].first, channelid);
}

void MythTV_Backend_Proxy::FrontendConnected(const char *host)
{
	PLUTO_SAFETY_LOCK( mutexData, m_mutexInternalData );
}

void MythTV_Backend_Proxy::FrontendDisconnected(const char *host)
{
	PLUTO_SAFETY_LOCK( mutexData, m_mutexInternalData );
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void MythTV_Backend_Proxy::SomeFunction()
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



//<-dceag-c264-b->

	/** @brief COMMAND: #264 - Track Frontend At IP */
	/** Send this to the Myth Backend to track the source IP provided as the device with IP, */
		/** @param #2 PK_Device */
			/** The device to track */
		/** @param #58 IP Address */
			/** The IP Address at which that device exists. */

void MythTV_Backend_Proxy::CMD_Track_Frontend_At_IP(int iPK_Device,string sIP_Address,string &sCMD_Result,Message *pMessage)
//<-dceag-c264-e->
{
	PLUTO_SAFETY_LOCK( mutexData, m_mutexInternalData );

	m_mapAddressToDeviceState[sIP_Address].first = iPK_Device;

	if ( m_mapAddressToDeviceState[sIP_Address].second != 0 )
		EVENT_MythTV_Channel_Changed(iPK_Device, m_mapAddressToDeviceState[sIP_Address].second);
}
//<-dceag-getconfig-b->
bool MythTV_Backend_Proxy::GetConfig()
{
	if( !MythTV_Backend_Proxy_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
}

//<-dceag-createinst-b->
MythTV_Backend_Proxy_Command *Create_MythTV_Backend_Proxy(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new MythTV_Backend_Proxy(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->
