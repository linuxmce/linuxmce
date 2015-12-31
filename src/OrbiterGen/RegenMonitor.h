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
#ifndef RegenMonitor_h
#define RegenMonitor_h

#include "PlutoUtils/DBHelper.h"

class RegenMonitor
{
	DBHelper *m_pDBHelper,*m_pDBHelper_Media;
	class Row_Room *m_pRow_Room;
	class Row_EntertainArea *m_pRow_EntertainArea;
public:

	RegenMonitor(DBHelper *pDBHelper,DBHelper *pDBHelper_Media) { m_pDBHelper=pDBHelper; m_pDBHelper_Media=pDBHelper_Media; m_pRow_EntertainArea=NULL; m_pRow_Room=NULL; }
	void SetRoomEA(Row_Room *pRow_Room,Row_EntertainArea *pRow_EntertainArea) { m_pRow_Room=pRow_Room;  m_pRow_EntertainArea=pRow_EntertainArea;}

	bool CachedVersionOK(string sString);

	string QueryAsModString(string sSQL);

	// This returns a string that shows the current status of this array so it can be 
	// compared to see if the array has changed.  For example, if the array is lighting scenarios (1), and 
	// the current room is 3 and the last psc_mod (timestamp) for rooms is 20050320 and for commandgroup_d is 20050321
	// the string might be: 1,3,20050320,20050321
	// If it returns a single *, that means the string cannot be determined for some reason and the screen
	// be regenerated
	string GetModInfo_Array(int PK_Array);

	// Some specific helper functions for the various types, called by GetModInfo
	string GetModInfo_RoomScenario(int PK_Array,int PK_Room);
	string GetModInfo_EntAreaScenario(int PK_Array,int PK_EntArea);
	string GetModInfo_DeviceCategory(int PK_DeviceCategory);
	string GetModInfo_Users();
	string GetModInfo_Locations();
	string GetModInfo_FloorplanArray();
	string GetModInfo_Floorplan(int PK_FloorplanType);
	string GetModInfo_Genres();

	// These return strings reflecting the state of all devices and rooms (to indicate if a reload is necessary)
	// and all scenarios/floorplans (to indicate if an orbitergen is necessary)
	string AllDevicesRooms();
	string AllScenariosFloorplans();
};

#endif
