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
#ifndef QML_Light_Switch_h
#define QML_Light_Switch_h

//	DCE Implemenation for #2181 QML Light Switch

#include "../Gen_Devices/QML_Light_SwitchBase.h"
#include <switchmanager.h>
//<-dceag-d-e->
class switchManager;

//<-dceag-decl-b->
namespace DCE
{
	class QML_Light_Switch : public QML_Light_Switch_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables
                switchManager *dceLightSwitch;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		QML_Light_Switch(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~QML_Light_Switch();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		QML_Light_Switch(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Floorplan_Info();
	int DATA_Get_PK_FloorplanObjectType();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #184 - Set Level */
	/** Sets the brightness of the light.  The value is 0 for off, up to 100 for full brightness. Several devices such as Drapes Switch can accept relative levels (+1 to step forward/-1 to step backword). */
		/** @param #76 Level */
			/** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

	virtual void CMD_Set_Level(string sLevel) { string sCMD_Result; CMD_Set_Level(sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage);


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
