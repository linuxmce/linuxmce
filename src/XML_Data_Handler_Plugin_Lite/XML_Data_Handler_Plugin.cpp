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
#include "XML_Data_Handler_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
XML_Data_Handler_Plugin::XML_Data_Handler_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: XML_Data_Handler_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_XML_Data_Factory(this)
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
XML_Data_Handler_Plugin::~XML_Data_Handler_Plugin()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool XML_Data_Handler_Plugin::GetConfig()
{
	if( !XML_Data_Handler_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool XML_Data_Handler_Plugin::Register()
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
void XML_Data_Handler_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void XML_Data_Handler_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c869-b->

	/** @brief COMMAND: #869 - Request XML Data */
	/** Requests data in XML format.  This request will get routed to a handler that registered for the given source */
		/** @param #15 DataGrid ID */
			/** The name of the datagrid to generate.  This corresponds to the id the data generator registered with */
		/** @param #109 Data String */
			/** The resulting data in XML Format */
		/** @param #202 Parameters */
			/** Incoming parameters in xml format. */

void XML_Data_Handler_Plugin::CMD_Request_XML_Data(string sDataGrid_ID,string sParameters,string *sData_String,string &sCMD_Result,Message *pMessage)
//<-dceag-c869-e->
{
	m_XML_Data_Factory.Populate(pMessage->m_dwPK_Device_From,sDataGrid_ID,sParameters,sData_String);
}

