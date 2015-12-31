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
#ifndef WizardLogic_h
#define WizardLogic_h

//-----------------------------------------------------------------------------------------------------
#include "PlutoUtils/DBHelper.h"
//-----------------------------------------------------------------------------------------------------

class Row_Device;

class WizardLogic : public DBHelper
{
private:
	friend class ScreenHandler;
	friend class OSDScreenHandler;
	long m_nPK_Device_TVProvider_External,m_nPK_Device_TV,m_nPK_Device_Receiver,
		m_nPK_Command_Input_Video_On_TV;
	bool m_bUsingReceiverForVideo;

	long m_nPK_Device_Last_AV, m_nPK_Device_AV_Installed;

	long m_nPK_Device_AlarmPanel,m_nPK_Device_Lighting; 

	string GetDeviceStatus(long nPK_Device);
	deque< pair<int,string> > m_dequeNumLights;
	deque< pair<int,string> > m_dequeNumSensors;

	int m_dwPK_Manufacturer,m_dwPK_DeviceTemplate;

protected:
	class Orbiter *m_pOrbiter;
	class UserUtils *m_pUserUtils;
public:
	list< pair<int,string> > m_listRoomTypes;  // In the order to display them
	map<int,string> m_mapRoomTypes; // For fast lookup

	WizardLogic(Orbiter *pOrbiter);
	virtual ~WizardLogic();

	bool Setup();

	/*
		GREETING
	*/

	bool HouseAlreadySetup();
	bool HasRemoteControl(bool bPopulateListOfOptions=true);

	/*
		SETUP USERS
	*/
	bool AlreadyHasUsers();
	bool AlreadyHasRooms();
	int AddUser(string sUsername);
	void RemoveUser(int PK_User);

	/*
		SETUP LOCATION
	*/
	int GetCountry();
	void SetCountry(int PK_Country);
	string GetCityRegion();
	bool SetLocation(string sLocation);
	bool GetLocation();
	bool SetPostalCode(string PostalCode);
	int GetPostalCode();
	void SetLongLat(string Latitude,string Longitude);


	/*
		SETUP ROOMS
	*/
	// Get a map of RoomTypes to number of Rooms of that Type, and the total number of rooms
	string GetRoomTypeName(int PK_RoomType);
	int PreSeedRoomInfo( map<int, int > &mapRooms ); // Call this to pre-populate the room info
	void ProcessUpdatedRoomInfo( map<int, int > &mapRooms ); // Call this to process the updated info
	void RemoveRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired ); // Helper function
	void AddRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired); // Helper function
	void AddRoomOfType(int PK_RoomType); //Helper function
	void RemoveRoomOfType(int PK_RoomType); //Helper function
	void ChangeRoomName(int PK_Room,string sName);
	void SetExternalDeviceInRoom(int PK_Device, string sPK_Room);
	int WhatRoomIsThisDeviceIn(int PK_Device);

	/*
		SETUP A/V EQUIPMENT
	*/


	int AddDevice(int PK_DeviceTemplate, string sDeviceDataList = "", long PK_Device_ControlledVia = 0);
	void SetAvPath(int PK_Device_From,int PK_Device_To,int PK_Pipe,int PK_Command_Input);
	bool GetAvPath(int PK_Device_From,long &PK_Device_To,int PK_Pipe,long &PK_Command_Input,string &sDescription,string &sCommandDescription);
	void AddExternalTuner(int PK_Device_Tuner);

	/*
		SETUP LIGHTING
	*/
	int GetNumLights();

	/*
		UTILITIES
	*/

	string Installation_get();
	void DeleteDevicesInThisRoomOfType(int PK_DeviceCategory);
	void AddExternalTuner(long something) {}
	void DeleteDevicesInThisRoomOfType(long something) {}
	int FindFirstDeviceInCategory(int PK_DeviceCategory,int PK_Device_RelatedTo,string *sDescription=NULL);
	int FindManufacturer(string sName);
	int FindModel(int PK_DeviceCategory,string sModel);
	void FindPnpDevices(string sPK_DeviceCategory);

	void SetPVRSoftware(char PVRSoftware);
	bool AnyPVRSoftwareInstalled();

	/*
		ALARM PANEL
	*/
	int GetNumSensors();

	string GetDeviceName(string sPK_Device);
	void SetDeviceName(string sPK_Device, string sName);

	long GetDeviceTemplateForDevice(string sPK_Device);
	void ChangeDeviceTemplateForDevice(string sPK_Device, string sFK_DeviceTemplate);

	long GetRoomForDevice(string sPK_Device);
	void SetRoomForDevice(string sPK_Device, string sFK_Room);

	int GetTopMostDevice(int PK_Device);
};
//-----------------------------------------------------------------------------------------------------
#endif
// WizardLogic_h
