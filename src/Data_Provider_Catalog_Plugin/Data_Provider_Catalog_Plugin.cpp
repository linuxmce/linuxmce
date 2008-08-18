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
#include "Data_Provider_Catalog_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DP_Catalog.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Data_Provider_Catalog_Plugin::Data_Provider_Catalog_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Data_Provider_Catalog_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDP_Catalog = new DP_Catalog(this);
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Data_Provider_Catalog_Plugin::~Data_Provider_Catalog_Plugin()
//<-dceag-dest-e->
{
	delete m_pDP_Catalog;	
}

//<-dceag-getconfig-b->
bool Data_Provider_Catalog_Plugin::GetConfig()
{
	if( !Data_Provider_Catalog_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Data_Provider_Catalog_Plugin::Register()
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
void Data_Provider_Catalog_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Data_Provider_Catalog_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c970-b->

	/** @brief COMMAND: #970 - Register Data Provider */
	/** Register a data provider in the catalog */
		/** @param #2 PK_Device */
			/** The device that handles the type of data */
		/** @param #10 ID */
			/** The ID of the data source.  If the last character is a * (wildcard) then it will provide all data sources that start with this ID */
		/** @param #279 Format Input */
			/** The format for input parameters.  * means match any request. */
		/** @param #280 Format Output */
			/** The format for output data.  * means match any request. */

void Data_Provider_Catalog_Plugin::CMD_Register_Data_Provider(int iPK_Device,string sID,string sFormat_Input,string sFormat_Output,string &sCMD_Result,Message *pMessage)
//<-dceag-c970-e->
{
	m_pDP_Catalog->Register_Data_Provider(iPK_Device,sID,sFormat_Input,sFormat_Output);
}

//<-dceag-c971-b->

	/** @brief COMMAND: #971 - Request Data Provider */
	/** Request data from a provider registered in the catalog */
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

void Data_Provider_Catalog_Plugin::CMD_Request_Data_Provider(string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String,string &sCMD_Result,Message *pMessage)
//<-dceag-c971-e->
{
	m_pDP_Catalog->Request_Data_Provider(pMessage->m_dwPK_Device_From, sID, sParameters, bMultiple, sFormat_Input, sFormat_Output, sCache, iValue, sData_String);
}
