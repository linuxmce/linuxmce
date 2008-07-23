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
#ifndef Basic_XML_Data_Source_Plugin_h
#define Basic_XML_Data_Source_Plugin_h

//	DCE Implemenation for #1869 Basic XML Data Source Plug-in

#include "Gen_Devices/Basic_XML_Data_Source_PluginBase.h"
//<-dceag-d-e->

//#include "pluto_main/Database_pluto_main.h"

//<-dceag-decl-b->
namespace DCE
{
	class Basic_XML_Data_Source_Plugin : public Basic_XML_Data_Source_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables
		class XML_Data_Handler_Plugin *m_pXML_Data_Handler_Plugin;
		//Database_pluto_main *m_pDatabase_pluto_main;
		class XML_Data_InstantiationInfo_Basic *m_pXML_Data_InstantiationInfo_Basic;

		// Private methods
//public:
//		// Public member variables
//		Database_pluto_main *m_pDatabase_pluto_main_get() { return m_pDatabase_pluto_main; }

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Basic_XML_Data_Source_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Basic_XML_Data_Source_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Basic_XML_Data_Source_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_PortChannel_Number();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #192 - On */
	/** Turns Lights ON */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #193 - Off */
	/** Turns Lights OFF */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

	virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
