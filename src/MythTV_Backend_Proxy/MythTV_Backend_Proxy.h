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
#ifndef MythTV_Backend_Proxy_h
#define MythTV_Backend_Proxy_h

//	DCE Implemenation for #63 MythTV Backend Proxy

#include "Gen_Devices/MythTV_Backend_ProxyBase.h"
//<-dceag-d-e->

#include "masterproxyserver.h"

#include <pthread.h>

#include <map>
#include <string>

//<-dceag-decl-b->!
namespace DCE
{
	class MythTV_Backend_Proxy : public MythTV_Backend_Proxy_Command, public MYTHTV::ProxyEventHandler
	{
//<-dceag-decl-e->
		// Private member variables
	private:
		pthread_mutexattr_t m_MutexAttr;
		pluto_pthread_mutex_t m_mutexInternalData;

		std::map<std::string, std::pair<int, int> > m_mapAddressToDeviceState;

		// Private methods
	public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		MythTV_Backend_Proxy(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~MythTV_Backend_Proxy();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual bool Connect(int iPK_DeviceTemplate);


//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		MythTV_Backend_Proxy(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

protected:
		void ChannelChanged(const char *host, int channelid);
		void FrontendConnected(const char *host);
		void FrontendDisconnected(const char *host);

private:
		MYTHTV::MasterProxyServer proxy_;


public:
//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_MythTV_Channel_Changed(int iPK_Device,int iMythTV_ChannelID);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #264 - Track Frontend At IP */
	/** Send this to the Myth Backend to track the source IP provided as the device with IP, */
		/** @param #2 PK_Device */
			/** The device to track */
		/** @param #58 IP Address */
			/** The IP Address at which that device exists. */

	virtual void CMD_Track_Frontend_At_IP(int iPK_Device,string sIP_Address) { string sCMD_Result; CMD_Track_Frontend_At_IP(iPK_Device,sIP_Address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Track_Frontend_At_IP(int iPK_Device,string sIP_Address,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
