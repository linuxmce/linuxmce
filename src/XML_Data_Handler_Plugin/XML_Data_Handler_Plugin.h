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
#ifndef XML_Data_Handler_Plugin_h
#define XML_Data_Handler_Plugin_h

//	DCE Implemenation for #1868 XML Data Handler Plug-in

#include "Gen_Devices/XML_Data_Handler_PluginBase.h"
//<-dceag-d-e->

#include "XML_Data_Factory.h"

//<-dceag-decl-b->
namespace DCE
{
	class XML_Data_Handler_Plugin : public XML_Data_Handler_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
		XML_Data_Factory m_XML_Data_Factory;
public:
		// Public member variables
		XML_Data_Factory &m_XML_Data_Factory_get() { return m_XML_Data_Factory; }

//<-dceag-const-b->
public:
		// Constructors/Destructor
		XML_Data_Handler_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~XML_Data_Handler_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->!

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


	/** @brief COMMAND: #869 - Request XML Data */
	/** Requests data in XML format.  This request will get routed to a handler that registered for the given source */
		/** @param #15 DataGrid ID */
			/** The name of the datagrid to generate.  This corresponds to the id the data generator registered with */
		/** @param #109 Data String */
			/** The resulting data in XML Format */
		/** @param #202 Parameters */
			/** Incoming parameters in xml format. */

	virtual void CMD_Request_XML_Data(string sDataGrid_ID,string sParameters,string *sData_String) { string sCMD_Result; CMD_Request_XML_Data(sDataGrid_ID.c_str(),sParameters.c_str(),sData_String,sCMD_Result,NULL);};
	virtual void CMD_Request_XML_Data(string sDataGrid_ID,string sParameters,string *sData_String,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
