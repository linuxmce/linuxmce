/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Cloud_Interface_h
#define Cloud_Interface_h

//	DCE Implemenation for #2315 Cloud Interface

#include "Gen_Devices/Cloud_InterfaceBase.h"

//<-dceag-d-e->
#include "QObject"
#include "QVariant"
//<-dceag-decl-b->
namespace DCE
{
    class Cloud_Interface : public QObject, public Cloud_Interface_Command
	{
        Q_OBJECT
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Cloud_Interface(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Cloud_Interface();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Cloud_Interface(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Configuration();

			*****EVENT***** accessors inherited from base class
	void EVENT_Download_Config_Done(string sError_Message);
	void EVENT_UserPassword_Mismatch(int iPK_Device,string sComment);

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
            signals:
        void dceReady();
        void commandForPlugin(int dt, QVariant cmd);


    private:
      QString m_configurationString;

	};

//<-dceag-end-b->



}
#endif
//<-dceag-end-e->
