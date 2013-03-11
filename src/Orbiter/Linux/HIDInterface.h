/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#ifndef PlutoHIDInterface_h
#define PlutoHIDInterface_h

#include "../Orbiter.h"
#define HAVE_STDBOOL_H
extern "C" 
{
		#include <linux/hid.h>
		#include <usb.h>
}

void *ProcessHIDEvents(void *p);

class PlutoHIDInterface
{
private:
	enum MouseStartStop { mssNone, mssStart, mssStop } m_MouseStartStop;

    usb_dev_handle *m_p_usb_dev_handle;
	bool m_bRunning;
	pluto_pthread_mutex_t m_HIDMutex;  // This will also protect the callback map
	int m_iRemoteID,m_iPK_Device_Remote;
	/*
	map<int,int> m_mapRemoteID_Device; // Map of known remote id's to device id's
	map<string,int> m_mapSerialNumber_RemoteID; // Map of known serial number's to remote id's
	int m_mapRemoteID_Device_Find(int RemoteID)	{ map<int,int>::iterator it = m_mapRemoteID_Device.find(RemoteID); return it==m_mapRemoteID_Device.end() ? NULL : (*it).second; }
	int m_mapSerialNumber_RemoteID_Find(string sSerialNumber) { map<string,int>::iterator it = m_mapSerialNumber_RemoteID.find(sSerialNumber); return it==m_mapSerialNumber_RemoteID.end() ? NULL : (*it).second; }
	*/
	int m_iHoldingDownButton; // The button currently being held down
	int m_iRepeat; // How many times the last button has been pressed in a row
	int m_iLastButtonPress; // The last button pressed
	time_t m_tLastButtonPress; // The time the last button was pressed
	Orbiter *m_pOrbiter;

private:
	void modules_remove();
	void modules_reload();

public:
	PlutoHIDInterface(Orbiter *pOrbiter);
	void ProcessHIDEvents();

	bool ProcessBindRequest(char *inPacket);
	bool ProcessHIDButton(char *inPacket);
	bool SetActiveRemote(int iRemoteID,bool bFollowMe);
	void StartMouse() { PLUTO_SAFETY_LOCK(vm,m_pOrbiter->m_VariableMutex); m_MouseStartStop=mssStart; }
	void StopMouse() { PLUTO_SAFETY_LOCK(vm,m_pOrbiter->m_VariableMutex); m_MouseStartStop=mssStop; }
	bool Rebind();
	bool DoStartMouse();
	bool DoStopMouse();
	void LegacyBind();
};

#endif // PlutoHIDInterface_h
