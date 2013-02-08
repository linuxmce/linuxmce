/*
     Copyright (C) 2009 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "RPC_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"

class Router *myRouter;
class RPC_Plugin *myPlugin;

struct mg_context       *ctx;
int data=0;

static void show_index(struct mg_connection *conn, const struct mg_request_info *request_info, void *user_data)
{
	mg_printf(conn, "%s",
		"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
		"<html><body><h1>Welcome to DCERouter/RPC-Plugin</h1>");
	mg_printf(conn, "%s", "<a href=\"/status\">/status</a> - device status<br>");
	mg_printf(conn, "%s", "<a href=\"/rooms\">/rooms</a> - rooms");
	mg_printf(conn, "%s", "</body></html>");
}


static void show_status (struct mg_connection *conn, const struct mg_request_info *request_info, void *user_data) {
	mg_printf(conn, "%s", "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	mg_printf(conn, "%s", "ID|Room|Category|Template|Description|State\n");

	// iterate over all devices
	for(map<int,class DeviceData_Router *>::const_iterator it=myRouter->m_mapDeviceData_Router_get()->begin();it!=myRouter->m_mapDeviceData_Router_get()->end();++it) {
		DeviceData_Router *pDeviceData_Router = (*it).second; 
		if( !pDeviceData_Router ) {
		} else {
			mg_printf(conn, "%i|%i|%i|%i|%s|%s\n", pDeviceData_Router->m_dwPK_Device,pDeviceData_Router->m_dwPK_Room,pDeviceData_Router->m_dwPK_DeviceCategory,pDeviceData_Router->m_dwPK_DeviceTemplate,pDeviceData_Router->m_sDescription.c_str(),pDeviceData_Router->m_sState_get().c_str());
		}
	}
}

static void show_rooms (struct mg_connection *conn, const struct mg_request_info *request_info, void *user_data) {
	mg_printf(conn, "%s", "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	mg_printf(conn, "%s", "ID|Description\n");

	for(map<int,class Room *>::const_iterator it=myRouter->m_mapRoom_get()->begin();it!=myRouter->m_mapRoom_get()->end();++it) {
		Room *pRoom = (*it).second;
		if ( !pRoom ) {
		} else {
			mg_printf(conn, "%i|%s\n", pRoom->m_dwPK_Room, pRoom->m_sDescription.c_str());
		}		
	}
}


static void json_data (struct mg_connection *conn, const struct mg_request_info *request_info, void *user_data) {

	mg_printf(conn, "%s", "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");

	mg_printf(conn, "%s (\n", mg_get_var(conn, "callback") );
	mg_printf(conn, "%s", "{ \"devices\": [\n");
	for(map<int,class DeviceData_Router *>::const_iterator it=myRouter->m_mapDeviceData_Router_get()->begin();it!=myRouter->m_mapDeviceData_Router_get()->end();++it) {
		DeviceData_Router *pDeviceData_Router = (*it).second; 
		if( !pDeviceData_Router ) {
		} else {
			mg_printf(conn, "{ \"id\": %i, \"room\": %i, \"category\": %i, \"template\": %i, \"description\": \"%s\", \"state\": \"%s\" }", pDeviceData_Router->m_dwPK_Device,pDeviceData_Router->m_dwPK_Room,pDeviceData_Router->m_dwPK_DeviceCategory,pDeviceData_Router->m_dwPK_DeviceTemplate,pDeviceData_Router->m_sDescription.c_str(),pDeviceData_Router->m_sState_get().c_str());
			if (it == myRouter->m_mapDeviceData_Router_get()->end() ) {
				mg_printf(conn, "\n");			
			} else {
				mg_printf(conn, ",\n");			
			}
		}
	}
	mg_printf(conn, "%s", "] } );\n");
} 

static void control_device (struct mg_connection *conn, const struct mg_request_info *request_info, void *user_data) {
	char *device, *command, *level;
	
	device = mg_get_var(conn, "device");
	command = mg_get_var(conn, "command");
	level = mg_get_var(conn, "level");
	mg_printf(conn, "%s", "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
	mg_printf(conn, "%s - %s", device, command);
	myPlugin->SendDCEMessage(device, command, level);


}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
RPC_Plugin::RPC_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: RPC_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pRouter = pRouter;
	myRouter = pRouter;
	myPlugin = this;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
RPC_Plugin::RPC_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: RPC_Plugin_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
RPC_Plugin::~RPC_Plugin()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool RPC_Plugin::GetConfig()
{
	if( !RPC_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	// start web server
	ctx = mg_start();
	mg_set_option(ctx, "ports", "8088");
	mg_set_uri_callback(ctx, "/", &show_index, NULL);
	mg_set_uri_callback(ctx, "/status", &show_status, NULL);
	mg_set_uri_callback(ctx, "/rooms", &show_rooms, NULL);
	mg_set_uri_callback(ctx, "/control", &control_device, NULL);
	mg_set_uri_callback(ctx, "/jsondata", &json_data, NULL);
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool RPC_Plugin::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
RPC_Plugin_Command *Create_RPC_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new RPC_Plugin(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void RPC_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void RPC_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void RPC_Plugin::SomeFunction()
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

void DCE::RPC_Plugin::SendDCEMessage(char *device, char *command, char *parameter) {
	// printf("DCE Callback: %s %s %s\n", device, command, parameter);	
	Message *pMessageOut = NULL;
	if ((device != NULL) && (command != NULL)) {
		if (strncmp(command, "on", 2) == 0) {
			pMessageOut = new Message (DEVICEID_MESSAGESEND, atoi(device), PRIORITY_NORMAL, MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST,0);
		} else if (strncmp(command, "off", 3) == 0) {
			pMessageOut = new Message (DEVICEID_MESSAGESEND, atoi(device), PRIORITY_NORMAL, MESSAGETYPE_COMMAND,COMMAND_Generic_Off_CONST,0);
		} else if (strncmp(command, "setlevel", 8) == 0) {
			if (parameter != NULL) { 
				pMessageOut = new Message (DEVICEID_MESSAGESEND, atoi(device), PRIORITY_NORMAL, MESSAGETYPE_COMMAND,COMMAND_Set_Level_CONST,1,COMMANDPARAMETER_Level_CONST,parameter);
			}

		}
	}
	if (pMessageOut != NULL) {
		SendMessageToRouter(pMessageOut);
	}

}
