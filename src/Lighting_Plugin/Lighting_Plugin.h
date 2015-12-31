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
#ifndef Lighting_Plugin_h
#define Lighting_Plugin_h

//	DCE Implemenation for #32 Lighting Plug-in

#include "Gen_Devices/Lighting_PluginBase.h"
//<-dceag-d-e->

#include "DCE/DCERouter.h"
#include "Orbiter/Floorplan.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "Orbiter_Plugin/FollowMe_Device.h"
class Database_pluto_main;

typedef pair<long, long> longPair;

//<-dceag-decl-b->!
namespace DCE
{
	class Lighting_Plugin : public Lighting_Plugin_Command, public DataGridGeneratorPlugIn, public FloorplanInfoProvider, public FollowMe_Device, public AlarmEvent
	{
//<-dceag-decl-e->
	// Private member variables
		map<int,longPair> m_mapRoom_CommandGroup;
		ListDeviceData_Router *m_pListDeviceData_Router_Lights;

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Lighting_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Lighting_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	
	virtual void PrepareToDelete();
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;
    pluto_pthread_mutex_t m_LightingMutex; 
	class AlarmManager *m_pAlarmManager;
	int m_iCameraTimeout;

	// When the system temporarily turns on a light, such as to capture an image from a camera, it stores the light
	// in this map with a pair that is the time to restore the light to it's prior value, and a string that represents
	// the prior value in the same format as the 'state' (ie ON/50)
	map<int, pair<time_t,string> > m_mapLightsToRestore;
	// this map is a flag to avoid processing the first event received for a light that is turned on because of a camera
	// so we don't loose the restore value
	map<int, bool > m_mapLightsToRestoreEvents;

	bool DeviceState( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool LightingFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool LightingCommand( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool GetVideoFrame( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar);
	void PreprocessLightingMessage(DeviceData_Router *pDevice,Message *pMessage);
	int GetLightingLevel(DeviceData_Router *pDevice,int iLevel_Default=0);
	
	// Set the state of a light.  -1 = don't change the level, just the on/off
	void SetLightState(int PK_Device,bool bIsOn,int Level=-1, bool bRestore=true);
	
	void SetLightingAlarm(); // Find the next event in m_mapLightsToRestore and set the alarm callback
	void AlarmCallback(int id, void* param);  // Implementation for AlarmEvent

	// Datagrids
	class DataGridTable *LightingScenariosGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	// Follow-me
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Timeout();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #184 - Set Level */
	/** Went sent by an orbiter, the plugin will adjust all lights in the Orbiter's room */
		/** @param #76 Level */
			/** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

	virtual void CMD_Set_Level(string sLevel) { string sCMD_Result; CMD_Set_Level(sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
