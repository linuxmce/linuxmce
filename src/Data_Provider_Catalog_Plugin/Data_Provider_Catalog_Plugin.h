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
#ifndef Data_Provider_Catalog_Plugin_h
#define Data_Provider_Catalog_Plugin_h

//	DCE Implemenation for #1948 Data Provider Catalog Plug-in

#include "Gen_Devices/Data_Provider_Catalog_PluginBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Data_Provider_Catalog_Plugin : public Data_Provider_Catalog_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
		class DP_Catalog *m_pDP_Catalog;
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Data_Provider_Catalog_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Data_Provider_Catalog_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Data_Provider_Catalog_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_Register_Data_Provider(int iPK_Device,string sID,string sFormat_Input,string sFormat_Output) { string sCMD_Result; CMD_Register_Data_Provider(iPK_Device,sID.c_str(),sFormat_Input.c_str(),sFormat_Output.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Register_Data_Provider(int iPK_Device,string sID,string sFormat_Input,string sFormat_Output,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Request_Data_Provider(string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String) { string sCMD_Result; CMD_Request_Data_Provider(sID.c_str(),sParameters.c_str(),bMultiple,sFormat_Input.c_str(),sFormat_Output.c_str(),sCache.c_str(),iValue,sData_String,sCMD_Result,NULL);};
	virtual void CMD_Request_Data_Provider(string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
