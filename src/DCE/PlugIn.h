/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file PlugIn.h
 Header file for the PlugIn class.
 @todo notcommented
 */
#ifndef PlugIn_h
#define PlugIn_h

#include <string>
using namespace std;

#include "Command_Impl.h"

namespace DCE
{
    /**
    @class PlugIn
    This class ???
     */
	class PlugIn
	{
	public:
		class Router *m_pRouter;
		int m_dwPK_Device;

		PlugIn(class Router *pRouter) { m_pRouter=pRouter; };  // hack -- remove this
		PlugIn(int iPK_Device,class Router *pRouter) { m_dwPK_Device=iPK_Device; m_pRouter=pRouter; };

		// Not done yet
		//	void CreatedDevice(DeviceData_Router *pDevice) {};  // Create special, extra data for devices for overload like lighting info???

		virtual bool Register()=0;   // returns true if successful
		virtual int ID_get()=0;	// HACK - this changes now	 // returns the ID of the Plug-in
		virtual ::std::string Description_get()=0; // HACK	// Returns a textual description of the plug-in
		virtual ReceivedMessageResult ReceivedMessage(class Message *pMessage) { return false; } // HACK =0;

		// Some Plug-ins may want to get pointers to, or register with other plug-ins
		// Init will be called when all the Plug-in's have registered
		virtual void Init() {};
	};

	/*
	Plug-in callbacks
	*/

	// return false if DCE should stop processing the message
	typedef  bool (Command_Impl::*MessageInterceptorFn)(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);
}
#endif
