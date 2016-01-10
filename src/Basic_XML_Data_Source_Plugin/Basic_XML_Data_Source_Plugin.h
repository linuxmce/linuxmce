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

#include "pluto_main/Database_pluto_main.h"

//<-dceag-decl-b->
namespace DCE
{
	class Basic_XML_Data_Source_Plugin : public Basic_XML_Data_Source_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables
		class XML_Data_Handler_Plugin *m_pXML_Data_Handler_Plugin;
		Database_pluto_main *m_pDatabase_pluto_main;
		class XML_Data_InstantiationInfo_Basic *m_pXML_Data_InstantiationInfo_Basic;

		// Private methods
public:
		// Public member variables
		Database_pluto_main *m_pDatabase_pluto_main_get() { return m_pDatabase_pluto_main; }

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

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
