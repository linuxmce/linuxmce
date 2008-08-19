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
#ifndef Media_Data_Provider_h
#define Media_Data_Provider_h

//	DCE Implemenation for #1950 Media Data Provider

#include "Gen_Devices/Media_Data_ProviderBase.h"
//<-dceag-d-e->

#include "PlutoUtils/DBHelper.h"
#include "Data_Provider_Catalog_Plugin/Data_Provider_CallBack.h"
#include "Generic_Mysql_Provider.h"

namespace DCE
{
	class Media_Provider_InstantiationInfo : public Data_Provider_InstantiationInfo
	{
	public:
		DBHelper *m_pDBHelper;

		Media_Provider_InstantiationInfo(DBHelper *pDBHelper) { m_pDBHelper=pDBHelper; }
		
	};
};

//<-dceag-decl-b->
namespace DCE
{
	class Media_Data_Provider : public Media_Data_Provider_Command
	{
//<-dceag-decl-e->
		// Private member variables
		Media_Provider_InstantiationInfo *m_pMedia_Provider_InstantiationInfo;
		class Data_Provider_Factory *m_pData_Provider_Factory;
		Generic_Mysql_Provider *m_pGeneric_Mysql_Provider;
		DBHelper *m_pDBHelper;
		class HTTP_Request_Handler *m_pHTTP_Request_Handler;

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Media_Data_Provider(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Media_Data_Provider();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual void PostConnect();

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
