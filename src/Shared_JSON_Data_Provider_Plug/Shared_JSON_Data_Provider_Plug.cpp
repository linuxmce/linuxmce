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
#include "Shared_JSON_Data_Provider_Plug.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DPS_MainJSONData.h"
#include "../Data_Provider_Catalog_Plugin/Data_Provider_Factory.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Shared_JSON_Data_Provider_Plug::Shared_JSON_Data_Provider_Plug(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Shared_JSON_Data_Provider_Plug_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Shared_JSON_Data_Provider_Plug::~Shared_JSON_Data_Provider_Plug()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Shared_JSON_Data_Provider_Plug::GetConfig()
{
	if( !Shared_JSON_Data_Provider_Plug_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Shared_JSON_Data_Provider_Plug::Register()
//<-dceag-reg-e->
{
	m_pData_Provider_Factory = new Data_Provider_Factory(this);
	m_pJSON_Provider_InstantiationInfo = new JSON_Provider_InstantiationInfo(m_pRouter);

	m_pData_Provider_Factory->Register( DPS_MainJSONData_Devices::Instantiate, DPS_MainJSONData_Devices::DataID(), "*", "json", m_pJSON_Provider_InstantiationInfo, Data_Provider_Source_Info::cache_never, false );
//	m_pData_Provider_Factory->Register( CommandGroupList::Instantiate, CommandGroupList::DataID, m_pXML_Data_InstantiationInfo_Basic, XML_Data_Source_Info::cache_never, true );
	m_pData_Provider_Factory->RegisterCatalog();

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
void Shared_JSON_Data_Provider_Plug::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Shared_JSON_Data_Provider_Plug::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c971-b->

	/** @brief COMMAND: #971 - Request Data Provider */
	/** Request data from this provider */
		/** @param #10 ID */
			/** The ID of the data being requested */
		/** @param #48 Value */
			/** An number representing this particular request.  If Multiple is true, the command can be resent with the same Value to get the same output */
		/** @param #109 Data String */
			/** The resulting Data */
		/** @param #202 Parameters */
			/** The parameters for the query */
		/** @param #259 Multiple */
			/** if true, we may make multiple requests and pass back in the same output Value */
		/** @param #279 Format Input */
			/** The format for the parameters */
		/** @param #280 Format Output */
			/** The desired format to retrieve the data */
		/** @param #281 Cache */
			/** Empty = Use Default. Y=Keep a copy of the output in cache, N=Do not keep this result in cache */

void Shared_JSON_Data_Provider_Plug::CMD_Request_Data_Provider(string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String,string &sCMD_Result,Message *pMessage)
//<-dceag-c971-e->
{
	m_pData_Provider_Factory->Populate(pMessage->m_dwPK_Device_From, sID, sParameters, bMultiple, sFormat_Input, sFormat_Output, sCache, iValue, sData_String);
}


