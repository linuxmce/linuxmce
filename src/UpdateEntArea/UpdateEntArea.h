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
#ifndef UpdateEntArea_h
#define UpdateEntArea_h

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_Room.h"
#include "CommandGroupArray.h"
#include "CreateDevice/CreateDevice.h"

class Row_EventHandler;
class Row_Criteria;
class Row_Device_DeviceData;

namespace DefaultScenarios
{

typedef enum { lomNone=0, lomContainsMD=1, lomContainsOtherVideo=2, lomContainsAudio=3, 
	       lomContainsWorkstation=4 } LevelOfMedia;

typedef map<char,string> MapShortcuts;  // Map the characters (like 1,2,3) to their default shortcuts

class UpdateEntArea 
{
	int m_iPK_Installation;
	int m_dwPK_Device_MediaPlugIn,m_dwPK_Device_OrbiterPlugIn,m_dwPK_Device_LightingPlugIn,
		m_dwPK_Device_ClimatePlugIn,m_dwPK_Device_SecurityPlugIn,m_dwPK_Device_TelecomPlugIn,
		m_dwPK_Device_DCERouter;
	map<int, pair<LevelOfMedia, bool> > m_mapRoom_Media;  // For a room, what type of media is in it, and bool=true when it's manually configure
	map<int, LevelOfMedia > m_mapEnt_Area_Auto_Media;  // For an ent area that is automatic, what type of media is in it
	map<int, MapShortcuts *> m_mapOrbiter_Shortcuts; // Map a room to it's shortcuts

	CreateDevice *m_pCreateDevice;

public:
    Database_pluto_main *m_pDatabase_pluto_main ;

	UpdateEntArea();
	~UpdateEntArea();

	// Setup functions which must be called to prep things (in UpdateEntArea_Setup.cpp) 
	bool Connect(int PK_Installation,string host, string user, string pass, string db_name, int port);
	void FixMissingAutoCreateDevices();
	void GetMediaAndRooms();
	// Call this to setup the EA's.  It's mandatory before calling AddDefaultMediaCommands();
	void SetEAInRooms();

	// Utility functions called internally by the setup functions 
	void PutMDsChildrenInRoom(Row_Device *pRow_Device);
	void DeleteEntertainArea(Row_EntertainArea *pRow_EntertainArea);
	void AddMDsDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea);
	void AddMDsDevicesToEntArea(Row_Device *pRow_Device,Row_EntertainArea *pRow_EntertainArea); // Recursive version of above
        void AddWSsDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea);
        void AddWSsDevicesToEntArea(Row_Device *pRow_Device,Row_EntertainArea *pRow_EntertainArea); // Recursive version of above

	void AddAVDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea);

	// Misc utility functions
	void SetDeviceData(int PK_Device,int PK_DeviceData,string sValue);
	Row_Device_DeviceData *GetUnmodifiedDeviceData(int PK_Device,int PK_DeviceData);

	// Get all devices within a certain category and, if PK_Room!=0, in a room, and fill the results
	// into p_map_Device_Type, which is a map of PK_Device,PK_FloorplanObjectType, and if 
	// p_mapType is not NULL, is a map of PK_FloorplanObjectType to a list of PK_Device
	void GetDevicesTypes(int PK_DeviceCategory,int PK_Room,map<int,int> *p_map_Device_Type,map<int,list<int> > *p_mapType);
	void GetDevicesTypesAndRoomTypes(int PK_DeviceCategory,map<int,pair<int,int> > *p_map_Device_Type_RoomType);

	// This just calls all the default AddScenarios below
	void AddDefaultScenarios();
	void UpdateOrbiterShortcuts();  // Use m_mapOrbiter_Shortcuts to set the device data for all the orbiters

	// Call these to add default scenarios to all rooms/EA's
	void AddDefaultMediaScenarios();  // UpdateEntArea_Media.cpp
	void AddDefaultLightingScenarios();  // UpdateEntArea_Lighting.cpp
	void AddDefaultClimateScenarios();  // UpdateEntArea_Climate.cpp
	void AddDefaultTelecomScenarios();  // UpdateEntArea_Telecom.cpp
	void AddDefaultSecurityScenarios();  // UpdateEntArea_Security.cpp
	void AddDefaultEventHandlers();  // UpdateEntArea_Event.cpp

	// Call these to add default scenarios to specific rooms/EA's
	void AddDefaultMediaScenarios(Row_EntertainArea *pRow_EntertainArea);
	void AddDefaultLightingScenarios(Row_Room *pRow_Room);
	void AddDefaultClimateScenarios(Row_Room *pRow_Room);
	void AddDefaultTelecomScenarios(Row_Room *pRow_Room);
	void AddDefaultSecurityScenarios(Row_Room *pRow_Room);
	void AddDefaultEventHandlers(Row_Room *pRow_Room);

	// Helper functions for security
	void AddQuadCameraScenarios(Row_Room *pRow_Room,vector<Row_Device *> &vectRow_Device,CommandGroupArray &commandGroupArray);
	void AddSingleCameraScenarios(Row_Room *pRow_Room,vector<Row_Device *> &vectRow_Device,CommandGroupArray &commandGroupArray);
	void SetDefaultAnnouncementDevices();
	void SetDefaultNotifications();
	void AddShortcut(int PK_Room, char cCharacter, string sMessage);
	void AddShortcut(int PK_Room, char cCharacter, int PK_CommandGroup);

	// Helper functions for events
	Row_EventHandler *CreateGenericEventHandler(CommandGroupArray &commandGroupArray,int PK_Template,int Parm1,int Parm2,string sDescription,int PK_Event);

	CommandGroup *CreateLeaveHomeCommandGroup(CommandGroupArray &commandGroupArray);
	Row_EventHandler *CreateLeaveHomeEventHandler(CommandGroupArray &commandGroupArray);
	Row_Criteria *SetLeaveHomeCriteria(Row_EventHandler *pRow_EventHandler);

	CommandGroup *CreateSunsetCommandGroup(CommandGroupArray &commandGroupArray);
	Row_EventHandler *CreateSunsetEventHandler(CommandGroupArray &commandGroupArray);
	CommandGroup *CreateSunriseCommandGroup(CommandGroupArray &commandGroupArray);
	Row_EventHandler *CreateSunriseEventHandler(CommandGroupArray &commandGroupArray);

	void ResetEventHandler_psc_mod(Row_EventHandler *pRow_EventHandler);

	CommandGroup *CreateWatchingMediaCommandGroup(CommandGroupArray &commandGroupArray,Row_Room *pRow_Room,int iIsWatching);
	Row_EventHandler *CreateWatchingMediaEventHandler(CommandGroupArray &commandGroupArray,Row_Room *pRow_Room,int iIsWatching);
	Row_Criteria *SetWatchingMediaCriteria(Row_EventHandler *pRow_EventHandler, long int FK_RoomType);

	CommandGroup *CreateSecurityAlertCommandGroup(CommandGroupArray &commandGroupArray,int PK_Event);

	// Misc helper functions for determining types of lights and rooms
	bool IsLight(int PK_FloorplanType);
	bool IsBlind(int PK_FloorplanType);
	bool IsPublicInteriorRoom(int PK_RoomType);
	bool IsInteriorRoom(int PK_RoomType);
	bool IsExterior(int PK_RoomType);

	void AddDevicesToEntArea(Row_Device *pRow_Device,Row_EntertainArea *pRow_EntertainArea,bool bOneEAOnly=true);
};

}

#endif
