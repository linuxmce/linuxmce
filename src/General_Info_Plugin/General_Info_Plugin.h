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
#ifndef General_Info_Plugin_h
#define General_Info_Plugin_h

//	DCE Implemenation for #27 General Info Plug-in

#include "Gen_Devices/General_Info_PluginBase.h"
//<-dceag-d-e->

#include "PostCreateOptions.h"
#include "DeviceData_Router.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "AlarmManager.h"

class Database_pluto_main;
class Database_pluto_media;
class Row_Device;

namespace DCE
{
	class OH_Orbiter;

	class LastApplication
	{
	public:
		string m_sName;
		int m_iPK_QuickStartTemplate;
	};
};

//<-dceag-decl-b->!
namespace DCE
{
	class General_Info_Plugin : public General_Info_Plugin_Command, public DataGridGeneratorPlugIn, public AlarmEvent
	{
//<-dceag-decl-e->
		typedef pair<int, string> pairPKDescription;
		typedef map<string, pairPKDescription> mapMacPKDescription;
//<-dceag-const-b->
public:
		// Constructors/Destructor
		General_Info_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~General_Info_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	virtual void PrepareToDelete();
	
	// Private member variables
	Database_pluto_main *m_pDatabase_pluto_main;
	Database_pluto_media *m_pDatabase_pluto_media;
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;
	class Event_Plugin *m_pEvent_Plugin;
	pluto_pthread_mutex_t m_GipMutex; // Other classes may need this
	pthread_mutexattr_t m_MutexAttr;
	bool m_bRerunConfigWhenDone;
	map<int,bool> m_mapMediaDirectors_PendingConfig;
	map<int, LastApplication *> m_mapLastApplication;
	LastApplication *m_mapLastApplication_Find(int PK_Device_MD) { map<int,class LastApplication *>::iterator it = m_mapLastApplication.find(PK_Device_MD); return it==m_mapLastApplication.end() ? NULL : (*it).second; }
	class PostCreateOptions *m_pPostCreateOptions;
	list<Row_Device *> m_listRow_Device_NewAdditions;
	int m_dwPK_Device_Prompting_For_A_Room;
	time_t m_tTimePromptedForRoom;
	bool m_bNewInstall;  // True when this is a new box that isn't setup yet
	class AlarmManager *m_pAlarmManager;

	// Private methods
	void SetNetBoot(DeviceData_Router *pDevice,bool bNetBoot);
	bool PendingConfigs()
	{
		PLUTO_SAFETY_LOCK(gm,m_GipMutex);
		for(map<int,bool>::iterator it=m_mapMediaDirectors_PendingConfig.begin();it!=m_mapMediaDirectors_PendingConfig.end();++it)
		{
			if( it->second )
				return true;
		}
		return false;
	}
	virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList=NULL);

	// Private methods
	list<pair<string, string> > GetUserBookmarks(string sPK_User);
	map<int,int> m_mapNewPnpDevicesWaitingForARoom,m_mapNewPnpDevicesWaitingForOptions;  // Map Device,Orbiter
	void SetRoomForDevice(Row_Device *pRow_Device,Row_Room *pRow_Room);
	void UpdateEntAreas();

public:
	// Public member variables

	class DataGridTable *PendingTasksGrid( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
	class DataGridTable *QuickStartApps( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
	class DataGridTable *MRUDocuments( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
	class DataGridTable *Rooms( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
	class DataGridTable *RoomTypes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *ChildrenInfo(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *AvailableSerialPorts(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *LightsTypes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *InstalledAVDevices(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *SensorType(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *TypesOfPhones( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *BookmarkList(string GridID, string Parms, void * ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *UsersGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *CountriesGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *CitiesGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *DevicesOfCategory( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *StorageDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *DeviceTemplatesOfCategory( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AddSoftware( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *FloorplanDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	//AV Wizard
	class DataGridTable *AVWhatDelay( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVDiscret( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	//AV Wizard - Input 
	class DataGridTable *AVInputNotListed( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVMediaType( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVMediaConnector(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVInputsAvaible( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	//AV Wizard - DSP Mode
	class DataGridTable *AVDSPMode (string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVDSPModeOrder( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	class DataGridTable *AVIRCodesSets( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );	
	class DataGridTable *IRCommands( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );	

	class DataGridTable *Discs( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *JukeBoxes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *HardDiscs( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *CompactFlashes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *NetworkStorage( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *JukeboxDrives( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *JukeboxSlots( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	// For the alarm callbacks
	virtual void AlarmCallback(int id, void* param);

/*
	void GetAppServerAndOsdForMD(DeviceData_Router *pDevice_MD,DeviceData_Router **pDevice_AppServer,DeviceData_Router **pDevice_Orbiter_OSD);
	Message *BuildMessageToSpawnApp(DeviceData_Router *pDevice_OrbiterRequesting,DeviceData_Router *pDevice_MD,DeviceData_Router *pDevice_AppServer,DeviceData_Router *pDevice_Orbiter_OSD,
		string sBinary,string sArguments,string sDescription,int PK_QuickStartTemplate=0);
*/
	bool ReportingChildDevices( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	void ReportingChildDevices_Offline( void *pVoid );
	bool LowSystemDiskSpace( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );


	// Helper functions
	void DoneCheckingForUpdates();
	Row_Device *ProcessChildDevice(Row_Device *pRow_Device,string sLine);
	void PromptUserToReloadAfterNewDevices();
	void ServiceRoomPromptRequests();

	virtual bool SafeToReload(string &sReason);

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


	/** @brief COMMAND: #68 - Get Device Data */
	/** Return the device data for a device. */
		/** @param #2 PK_Device */
			/** The device which you want data from */
		/** @param #5 Value To Assign */
			/** The value. */
		/** @param #52 PK_DeviceData */
			/** What parameter to get. */
		/** @param #53 UseDefault */
			/** Report the default value, rather than requesting a live value. */

	virtual void CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Device_Data(iPK_Device,iPK_DeviceData,bUseDefault,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #71 - Request File */
	/** Get the contents of a file from the server */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */

	virtual void CMD_Request_File(string sFilename,char **pData,int *iData_Size) { string sCMD_Result; CMD_Request_File(sFilename.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Request_File(string sFilename,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #239 - Request File And Checksum */
	/** Get the contents of a file from the server and the checksum of the file */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */
		/** @param #91 Checksum */
			/** The checksum of the file */
		/** @param #92 Checksum Only */
			/** If this is true, this command will return only the checksum of the file, Data will be null. */

	virtual void CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only) { string sCMD_Result; CMD_Request_File_And_Checksum(sFilename.c_str(),pData,iData_Size,sChecksum,bChecksum_Only,sCMD_Result,NULL);};
	virtual void CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #246 - Set Device Data */
	/** Gets the device data for a device */
		/** @param #2 PK_Device */
			/** The device to set */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #52 PK_DeviceData */
			/** The device data */

	virtual void CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData) { string sCMD_Result; CMD_Set_Device_Data(iPK_Device,sValue_To_Assign.c_str(),iPK_DeviceData,sCMD_Result,NULL);};
	virtual void CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #247 - Get Device State */
	/** Gets a device's current state */
		/** @param #2 PK_Device */
			/** The device id which you need information for. */
		/** @param #5 Value To Assign */
			/** The state data for the device. */

	virtual void CMD_Get_Device_State(int iPK_Device,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Device_State(iPK_Device,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Device_State(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #248 - Get Device Status */
	/** Gets the status for a device */
		/** @param #2 PK_Device */
			/** The device id which you need information for. */
		/** @param #5 Value To Assign */
			/** The status data for the device. */

	virtual void CMD_Get_Device_Status(int iPK_Device,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Device_Status(iPK_Device,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Device_Status(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #272 - Restart DCERouter */
	/** Causes DCERouter to exit and restart. */
		/** @param #21 Force */

	virtual void CMD_Restart_DCERouter(string sForce) { string sCMD_Result; CMD_Restart_DCERouter(sForce.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Restart_DCERouter(string sForce,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device, or if negative, this is a PK_PnpQueue and needs to be forwarded to plug and play plugin */
		/** @param #50 Name */
			/** If PK_Room is empty, a new room with this name will be created */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Room_For_Device(int iPK_Device,string sName,int iPK_Room) { string sCMD_Result; CMD_Set_Room_For_Device(iPK_Device,sName.c_str(),iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Room_For_Device(int iPK_Device,string sName,int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #322 - Wake Device */
	/** Sends a Wake on LAN to the specified device. */
		/** @param #2 PK_Device */
			/** The device to wake up */

	virtual void CMD_Wake_Device(int iPK_Device) { string sCMD_Result; CMD_Wake_Device(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Wake_Device(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #323 - Halt Device */
	/** Halts, or suspends, the given device. */
		/** @param #2 PK_Device */
			/** The device to halt */
		/** @param #21 Force */
			/** If Force is not specified this will do a suspend if the device supports suspend/resume, otherwise it will do a halt.  Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot */
		/** @param #47 Mac address */
			/** If PK_Device is not specified (is 0), we'll use the mac address to determine the device id */

	virtual void CMD_Halt_Device(int iPK_Device,string sForce,string sMac_address) { string sCMD_Result; CMD_Halt_Device(iPK_Device,sForce.c_str(),sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Halt_Device(int iPK_Device,string sForce,string sMac_address,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #365 - Get Room Description */
	/** Given a room or device ID, returns the description.  If device ID, also returns the room ID. */
		/** @param #2 PK_Device */
			/** The ID of the device.  Specify this or the room ID. */
		/** @param #9 Text */
			/** The description of the room */
		/** @param #57 PK_Room */
			/** The ID of the room.  Specify this or the device ID. */

	virtual void CMD_Get_Room_Description(int iPK_Device,string *sText,int *iPK_Room) { string sCMD_Result; CMD_Get_Room_Description(iPK_Device,sText,iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Get_Room_Description(int iPK_Device,string *sText,int *iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #371 - Is Daytime */
	/** Returns true or false to indicate if it is daytime (ie between sunrise and sunset) */
		/** @param #119 True/False */
			/** Returns true if it is daytime. */

	virtual void CMD_Is_Daytime(bool *bTrueFalse) { string sCMD_Result; CMD_Is_Daytime(bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Is_Daytime(bool *bTrueFalse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #395 - Check for updates */
	/** Check all PC's in the system to see if there are available updates on any of them by having all AppServer's run /usr/pluto/bin/Config_Device_Changes.sh */

	virtual void CMD_Check_for_updates() { string sCMD_Result; CMD_Check_for_updates(sCMD_Result,NULL);};
	virtual void CMD_Check_for_updates(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #396 - Check for updates done */
	/** An App Server finished running /usr/pluto/bin/Config_Device_Changes.sh and notifies the g.i. plugin. */
		/** @param #230 Failed */
			/** This is true if the script used to check for updates fails. */

	virtual void CMD_Check_for_updates_done(bool bFailed) { string sCMD_Result; CMD_Check_for_updates_done(bFailed,sCMD_Result,NULL);};
	virtual void CMD_Check_for_updates_done(bool bFailed,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #718 - Create Device */
	/** Creates a new device of the given template */
		/** @param #2 PK_Device */
			/** The new device number */
		/** @param #44 PK_DeviceTemplate */
			/** The template */
		/** @param #47 Mac address */
			/** The mac address */
		/** @param #57 PK_Room */
			/** The room for the device.  0=no room, -1=ask user */
		/** @param #58 IP Address */
			/** The IP of the device */
		/** @param #109 Data String */
			/** Extra device data to be assigned when creating the device. It should look like this:
devicedata_id1|devicedata_value1|devicedata_id2|devicedata_value2| etc. */
		/** @param #150 PK_DHCPDevice */
			/** Only needed if this is a dhcp pnp device */
		/** @param #156 PK_Device_ControlledVia */
			/** The controlled via */
		/** @param #163 Description */
			/** The name for the device */
		/** @param #198 PK_Orbiter */
			/** The orbiter which should be used to prompt the user for any extra information.  Zero means all orbiters */
		/** @param #201 PK_Device_Related */
			/** Will make the new device relate to this one if possible */

	virtual void CMD_Create_Device(int iPK_DeviceTemplate,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,int iPK_DHCPDevice,int iPK_Device_ControlledVia,string sDescription,int iPK_Orbiter,int iPK_Device_Related,int *iPK_Device) { string sCMD_Result; CMD_Create_Device(iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str(),iPK_DHCPDevice,iPK_Device_ControlledVia,sDescription.c_str(),iPK_Orbiter,iPK_Device_Related,iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Create_Device(int iPK_DeviceTemplate,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,int iPK_DHCPDevice,int iPK_Device_ControlledVia,string sDescription,int iPK_Orbiter,int iPK_Device_Related,int *iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #719 - Delete Device */
	/** Deletes a device */
		/** @param #2 PK_Device */
			/** The device to delete */

	virtual void CMD_Delete_Device(int iPK_Device) { string sCMD_Result; CMD_Delete_Device(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Delete_Device(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #752 - Check Mounts */
	/** Re-mount all network storage devices */

	virtual void CMD_Check_Mounts() { string sCMD_Result; CMD_Check_Mounts(sCMD_Result,NULL);};
	virtual void CMD_Check_Mounts(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #765 - Set Device Relations */
	/** Set which devices are related to which */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #103 List PK Device */
			/** The devices it relates to */
		/** @param #205 Reverse */
			/** If true, the device list are the source devices and the PK_Device is the relation */

	virtual void CMD_Set_Device_Relations(int iPK_Device,string sList_PK_Device,bool bReverse) { string sCMD_Result; CMD_Set_Device_Relations(iPK_Device,sList_PK_Device.c_str(),bReverse,sCMD_Result,NULL);};
	virtual void CMD_Set_Device_Relations(int iPK_Device,string sList_PK_Device,bool bReverse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #789 - Force Update Packages */
	/** For the system to upgrade */

	virtual void CMD_Force_Update_Packages() { string sCMD_Result; CMD_Force_Update_Packages(sCMD_Result,NULL);};
	virtual void CMD_Force_Update_Packages(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #791 - Set Enable Status */
	/** This command will enable or disable a device. A reload router will be needed in order to take effect. */
		/** @param #2 PK_Device */
			/** The id of the device to be enabled/disabled */
		/** @param #208 Enable */
			/** If this is true ( = 1), the device will be enabled. Otherwise, the device will be disabled. */

	virtual void CMD_Set_Enable_Status(int iPK_Device,bool bEnable) { string sCMD_Result; CMD_Set_Enable_Status(iPK_Device,bEnable,sCMD_Result,NULL);};
	virtual void CMD_Set_Enable_Status(int iPK_Device,bool bEnable,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #800 - InitAVDeviceTemplateSettings */
	/** Save default values in database for a deviceTemplate */
		/** @param #44 PK_DeviceTemplate */
			/** Template id */

	virtual void CMD_InitAVDeviceTemplateSettings(int iPK_DeviceTemplate) { string sCMD_Result; CMD_InitAVDeviceTemplateSettings(iPK_DeviceTemplate,sCMD_Result,NULL);};
	virtual void CMD_InitAVDeviceTemplateSettings(int iPK_DeviceTemplate,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #802 - Get Available Storage Device */
	/** The the available storage devices */
		/** @param #2 PK_Device */
			/** The device id for the NAS or internal drive being used.  If it will be stored on the Core's internal home directory, this will be the device id for the Core.  0 is no device can save the file. */
		/** @param #163 Description */
			/** The descripition for the device being used for the storage (ie Core, or a device name). */
		/** @param #219 Path */
			/** Returns the fully qualified path to prepend to the filename.  If this is empty, that means there is no device which can save this file */
		/** @param #222 Size */
			/** The expected size of the file in MB.  If specified, only a device with enough space and which can handle it (ie some NAS don't do big files).  If not specified, returns device with the most free space. */

	virtual void CMD_Get_Available_Storage_Device(int iSize,int *iPK_Device,string *sDescription,string *sPath) { string sCMD_Result; CMD_Get_Available_Storage_Device(iSize,iPK_Device,sDescription,sPath,sCMD_Result,NULL);};
	virtual void CMD_Get_Available_Storage_Device(int iSize,int *iPK_Device,string *sDescription,string *sPath,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #803 - Blacklist Internal Disk Drive */
	/** Adds an internal disk drive device to the list of blacklisted drives. */
		/** @param #156 PK_Device_ControlledVia */
			/** The 'computer' device that contains the blacklisted drive. */
		/** @param #223 Block Device */
			/** Linux block device representing the Internal Disk Drive. */

	virtual void CMD_Blacklist_Internal_Disk_Drive(int iPK_Device_ControlledVia,string sBlock_Device) { string sCMD_Result; CMD_Blacklist_Internal_Disk_Drive(iPK_Device_ControlledVia,sBlock_Device.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Blacklist_Internal_Disk_Drive(int iPK_Device_ControlledVia,string sBlock_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #808 - Get Unused Serial Ports */
	/** Get the unused serial ports */
		/** @param #2 PK_Device */
			/** The computer to get the ports for */
		/** @param #5 Value To Assign */
			/** A comma delimited list of ports */

	virtual void CMD_Get_Unused_Serial_Ports(int iPK_Device,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Unused_Serial_Ports(iPK_Device,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Unused_Serial_Ports(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #813 - Add Software */
	/** Install Software on media director */
		/** @param #2 PK_Device */
			/** The MD to install it on */
		/** @param #119 True/False */
			/** True = add the package, false=remove it */
		/** @param #229 PK_Software */
			/** ID of installing package */

	virtual void CMD_Add_Software(int iPK_Device,bool bTrueFalse,int iPK_Software) { string sCMD_Result; CMD_Add_Software(iPK_Device,bTrueFalse,iPK_Software,sCMD_Result,NULL);};
	virtual void CMD_Add_Software(int iPK_Device,bool bTrueFalse,int iPK_Software,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #833 - Get User Name */
	/** Given a PK_User, get the users name */
		/** @param #5 Value To Assign */
			/** The user's name */
		/** @param #17 PK_Users */
			/** The user id */

	virtual void CMD_Get_User_Name(int iPK_Users,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_User_Name(iPK_Users,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_User_Name(int iPK_Users,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #835 - Get Network Devices Shares */
	/** Get the list with shares for all network devices from this installation. */
		/** @param #244 Custom Response */
			/** Contains the following info:

device id, device ip, share name, user name, password, free space.

Delimiter: '\n' */

	virtual void CMD_Get_Network_Devices_Shares(char **pCustom_Response,int *iCustom_Response_Size) { string sCMD_Result; CMD_Get_Network_Devices_Shares(pCustom_Response,iCustom_Response_Size,sCMD_Result,NULL);};
	virtual void CMD_Get_Network_Devices_Shares(char **pCustom_Response,int *iCustom_Response_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #843 - RemoteAssistance_SetStatus */
	/** Change the state of Remote Assistance */
		/** @param #208 Enable */
			/** Desired state of Remote Assistance */

	virtual void CMD_RemoteAssistance_SetStatus(bool bEnable) { string sCMD_Result; CMD_RemoteAssistance_SetStatus(bEnable,sCMD_Result,NULL);};
	virtual void CMD_RemoteAssistance_SetStatus(bool bEnable,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #844 - RemoteAssistance_GetStatus */
	/** Get the state of Remote Assistance */
		/** @param #208 Enable */
			/** The state of Remote Assistance */

	virtual void CMD_RemoteAssistance_GetStatus(bool *bEnable) { string sCMD_Result; CMD_RemoteAssistance_GetStatus(bEnable,sCMD_Result,NULL);};
	virtual void CMD_RemoteAssistance_GetStatus(bool *bEnable,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

	virtual void CMD_Abort_Task(int iParameter_ID) { string sCMD_Result; CMD_Abort_Task(iParameter_ID,sCMD_Result,NULL);};
	virtual void CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #915 - Enable Device */
	/** Called when instead of creating a device, we re-enabled one that had been disabled and need to call the postcreateoptions again */
		/** @param #2 PK_Device */
			/** The device that was enabled */
		/** @param #198 PK_Orbiter */
			/** The Orbiter that did it, if any */

	virtual void CMD_Enable_Device(int iPK_Device,int iPK_Orbiter) { string sCMD_Result; CMD_Enable_Device(iPK_Device,iPK_Orbiter,sCMD_Result,NULL);};
	virtual void CMD_Enable_Device(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #934 - Get Home Symlink */
	/** Get home symlink */
		/** @param #219 Path */
			/** The physical path like /mnt/device/<deviceid> */
		/** @param #275 Symlink */
			/** The symlink like /home/public/data ... */

	virtual void CMD_Get_Home_Symlink(string sPath,string *sSymlink) { string sCMD_Result; CMD_Get_Home_Symlink(sPath.c_str(),sSymlink,sCMD_Result,NULL);};
	virtual void CMD_Get_Home_Symlink(string sPath,string *sSymlink,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #956 - Get Devices To Start */
	/** Get the list with devices to start. */
		/** @param #2 PK_Device */
			/** The parent device */
		/** @param #5 Value To Assign */
			/** A pipe delimited list like this: DeviceID1|TemplateName1|CommandLine1\nDeviceID2|DeviceTemplateName2|CommandLine2 etc */

	virtual void CMD_Get_Devices_To_Start(int iPK_Device,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Devices_To_Start(iPK_Device,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Devices_To_Start(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #957 - Update Device */
	/** Will update the description, ip, mac, device data */
		/** @param #2 PK_Device */
			/** Device ID */
		/** @param #47 Mac address */
			/** Mac address */
		/** @param #57 PK_Room */
			/** Room ID */
		/** @param #58 IP Address */
			/** IP address */
		/** @param #109 Data String */
			/** pipe delimited list with device data */
		/** @param #163 Description */
			/** Device description */

	virtual void CMD_Update_Device(int iPK_Device,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,string sDescription) { string sCMD_Result; CMD_Update_Device(iPK_Device,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str(),sDescription.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Device(int iPK_Device,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,string sDescription,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1075 - Send Email */
	/** Send an email in response to events. */
		/** @param #282 To */
			/** Who does the message go to? */
		/** @param #283 Subject */
			/** Synopsis of what happened. */
		/** @param #284 MessageBody */
			/** Details about the event. */
		/** @param #288 From */
			/** What is the sender's email address? */

	virtual void CMD_Send_Email(string sTo,string sSubject,string sMessageBody,string sFrom) { string sCMD_Result; CMD_Send_Email(sTo.c_str(),sSubject.c_str(),sMessageBody.c_str(),sFrom.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Email(string sTo,string sSubject,string sMessageBody,string sFrom,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1106 - Get Users */
	/** Get users */
		/** @param #5 Value To Assign */
			/** Returns the users in this system as a Pk\tNickName\nPk\tNickName\n etc. */

	virtual void CMD_Get_Users(string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Users(sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Users(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1107 - Get Rooms */
	/** Get the rooms */
		/** @param #5 Value To Assign */
			/** Returns the rooms in this installation as Pk\tDescription\nPk\tDescription\n etc. */

	virtual void CMD_Get_Rooms(string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Rooms(sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Rooms(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1108 - Get Scenarios */
	/** Get scenarios */
		/** @param #5 Value To Assign */
			/** Returns the scenarios for the specified parameters. */
		/** @param #45 PK_EntertainArea */
			/** Entertainment area to get scenarios for. Only valid for media scenarios. */
		/** @param #48 Value */
			/** Scenario to get. 1 = Lightning, 2= Climate, 3 = Security, 4 = Telecom, 5 = Media */
		/** @param #57 PK_Room */
			/** The Room to get the scenarios for. Required! */

	virtual void CMD_Get_Scenarios(string sPK_EntertainArea,int iValue,int iPK_Room,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Scenarios(sPK_EntertainArea.c_str(),iValue,iPK_Room,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Scenarios(string sPK_EntertainArea,int iValue,int iPK_Room,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1109 - Get Entertainment Areas */
	/** Get the entertainment areas */
		/** @param #5 Value To Assign */
			/** Returns the entertainment areas for this installation as Pk_EntArea\tFK_Room\tDescription\n etc. */

	virtual void CMD_Get_Entertainment_Areas(string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Entertainment_Areas(sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Entertainment_Areas(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	private:
		mapMacPKDescription m_mapMacPKDescription;
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
