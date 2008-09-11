/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//<-dceag-d-b->
#ifndef Climate_Plugin_h
#define Climate_Plugin_h

//	DCE Implemenation for #31 Climate Plug-in

#include "Gen_Devices/Climate_PluginBase.h"
//<-dceag-d-e->

namespace DCE
{
	class DeviceData_Router;
}

//<-dceag-decl-b->!
namespace DCE
{
	class Climate_Plugin : public Climate_Plugin_Command
	{
//<-dceag-decl-e->
	// Private member variables

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Climate_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Climate_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

	// Interceptors
	bool ClimateCommand( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	void PreprocessClimateMessage(DeviceData_Router *pDevice,Message *pMessage);
	
	int GetClimateLevel(DeviceData_Router *pDevice,int iLevel_Default);
	string GetTemperature(DeviceData_Router *pDevice);
	void SetStateValue(DeviceData_Router *pDevice, string sOn, string sMode, string sFan, string sSetPoint, string sTemp);
	void GetStateVar(DeviceData_Router *pDevice, string& sOn, string& sMode, string& sFan, string& sSetPoint, string& sTemp);

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
//<-dceag-const2-b->!
