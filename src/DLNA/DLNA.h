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
#ifndef DLNA_h
#define DLNA_h

//	DCE Implemenation for #2215 DLNA

#include "Gen_Devices/DLNABase.h"
//<-dceag-d-e->

#include <QtCore/QCoreApplication>
#include "EntertainArea.h"

namespace DCE {
	class DLNAEngine;
}
//<-dceag-decl-b->
namespace DCE
{
	class DLNA : public DLNA_Command
	{
//<-dceag-decl-e->
		// Private member variables
		QCoreApplication* m_pApp;
		DLNAEngine* m_pEngine;

		// Private methods
		map<int,EntertainArea*> m_mapEntertainAreas;
public:
		// Public member variables
		bool m_bEnableMediaRenderer, m_bEnableMediaServer, m_bEnableControlPoints;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		DLNA(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~DLNA();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		void SetQtApp(QCoreApplication* pApp) { m_pApp = pApp; }

		void OnQuit();
		void OnReload();
		bool LoadEntertainAreas();
		map<int, EntertainArea*>* GetEntertainAreas();

		bool MediaCommandIntercepted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		EntertainArea* FindEntertainAreaForDevice(int PK_Device);
		EntertainArea* FindEntertainArea(int PK_EntertainArea);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		DLNA(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
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
