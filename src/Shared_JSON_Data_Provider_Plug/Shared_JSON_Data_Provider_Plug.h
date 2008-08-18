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
#ifndef Shared_JSON_Data_Provider_Plug_h
#define Shared_JSON_Data_Provider_Plug_h

//	DCE Implemenation for #1949 Shared JSON Data Provider Plug

#include "Gen_Devices/Shared_JSON_Data_Provider_PlugBase.h"
//<-dceag-d-e->

#include "../Data_Provider_Catalog_Plugin/Data_Provider_CallBack.h"

namespace DCE
{
	class JSON_Provider_InstantiationInfo : public Data_Provider_InstantiationInfo
	{
	public:
		class Router *m_pRouter;

		JSON_Provider_InstantiationInfo(class Router *pRouter) { m_pRouter=pRouter; }
		
	};
};

//<-dceag-decl-b->
namespace DCE
{
	class Shared_JSON_Data_Provider_Plug : public Shared_JSON_Data_Provider_Plug_Command
	{
//<-dceag-decl-e->
		// Private member variables
		JSON_Provider_InstantiationInfo *m_pJSON_Provider_InstantiationInfo;
		class Data_Provider_Factory *m_pData_Provider_Factory;

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Shared_JSON_Data_Provider_Plug(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Shared_JSON_Data_Provider_Plug();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Shared_JSON_Data_Provider_Plug(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
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

	virtual void CMD_Request_Data_Provider(string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String) { string sCMD_Result; CMD_Request_Data_Provider(sID.c_str(),sParameters.c_str(),bMultiple,sFormat_Input.c_str(),sFormat_Output.c_str(),sCache.c_str(),iValue,sData_String,sCMD_Result,NULL);};
	virtual void CMD_Request_Data_Provider(string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
