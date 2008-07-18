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
#include "General_Info_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCE/DCERouter.h"
#include "DCERouter/Command_Data.h"
#include "DCERouter/Scene_Data.h"

Command_Data_CallBack::Command_Data_CallBack(Command_Data *pCommand_Data, time_t tTime, int PK_Device_From)
{
	m_pCommand_Data = new Command_Data();
	*m_pCommand_Data = *pCommand_Data;
	m_tTime = tTime;
	m_PK_Device_From = PK_Device_From;
}

Command_Data_CallBack::~Command_Data_CallBack()
{
	delete m_pCommand_Data;
}

struct ChildDeviceProcessing
{
	string m_sChildren;
	int m_nPK_Device;

	ChildDeviceProcessing(int nPK_Device, string sChildren) 
	{ 
		m_nPK_Device = nPK_Device; 
		m_sChildren = sChildren; 
	}
};

#define PROCESS_CHILD_DEVICES 1
#define PROCESS_DELAYED_EXECUTE_SCENARIOS	3

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
General_Info_Plugin::General_Info_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: General_Info_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pAlarmManager=NULL;
	m_pCallBack_ForExecuteScenarios=NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
General_Info_Plugin::~General_Info_Plugin()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool General_Info_Plugin::GetConfig()
{
	if( !General_Info_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(1);      // number of worker threads
	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool General_Info_Plugin::Register()
//<-dceag-reg-e->
{
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::ReportingChildDevices ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Reporting_Child_Devices_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::DeviceDetected ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_Detected_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::DeviceRemoved ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_Removed_CONST );

	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void General_Info_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void General_Info_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c68-b->

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

void General_Info_Plugin::CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c68-e->
{
	cout << "Need to implement command #68 - Get Device Data" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #52 - PK_DeviceData=" << iPK_DeviceData << endl;
	cout << "Parm #53 - UseDefault=" << bUseDefault << endl;
}

//<-dceag-c71-b->

	/** @brief COMMAND: #71 - Request File */
	/** Get the contents of a file from the server */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */

void General_Info_Plugin::CMD_Request_File(string sFilename,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c71-e->
{
	cout << "Need to implement command #71 - Request File" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
}

//<-dceag-c239-b->

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

void General_Info_Plugin::CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only,string &sCMD_Result,Message *pMessage)
//<-dceag-c239-e->
{
	LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "Request file from server: %s", sFilename.c_str());
	size_t Length = 0;
	char *c = FileUtils::ReadFileIntoBuffer(sFilename, Length);
	if( c==NULL && m_pRouter )
		c = FileUtils::ReadFileIntoBuffer(m_pRouter->sBasePath_get() + sFilename, Length);

	if(c==NULL) //file not found
	{
		LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "The requested file '%s' was not found", sFilename.c_str());
		return;
	}

	*iData_Size = (int) Length;
	*pData = c;
	*sChecksum = FileUtils::FileChecksum(*pData, *iData_Size);

	if(*bChecksum_Only)
	{
		*iData_Size = 0;

		if(NULL != *pData)
		{
			delete *pData;
			*pData = NULL;
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "General_Info_Plugin::CMD_Request_File_And_Checksum: file %s", sFilename.c_str());
}

//<-dceag-c246-b->

	/** @brief COMMAND: #246 - Set Device Data */
	/** Gets the device data for a device */
		/** @param #2 PK_Device */
			/** The device to set */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #52 PK_DeviceData */
			/** The device data */

void General_Info_Plugin::CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData,string &sCMD_Result,Message *pMessage)
//<-dceag-c246-e->
{
	DeviceData_Router *pDeviceData_Router = m_pRouter->DataLayer()->Device(iPK_Device);

	if(NULL == pDeviceData_Router)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to update device %d, it doesn't exist", iPK_Device);
		sCMD_Result = "ERROR: Device doesn't exist";
		return;
	}

	pDeviceData_Router->ReplaceParameter(iPK_DeviceData, sValue_To_Assign);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Updating device data %d with value %s", iPK_DeviceData, sValue_To_Assign.c_str());
	m_pRouter->DataLayer()->Save();
}

//<-dceag-c247-b->

	/** @brief COMMAND: #247 - Get Device State */
	/** Gets a device's current state */
		/** @param #2 PK_Device */
			/** The device id which you need information for. */
		/** @param #5 Value To Assign */
			/** The state data for the device. */

void General_Info_Plugin::CMD_Get_Device_State(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c247-e->
{
	if(iPK_Device)
	{
		DeviceData_Router *pDeviceData_Router = m_pRouter->DataLayer()->Device(iPK_Device);
		if( !pDeviceData_Router )
		{
			sCMD_Result = "BAD DEVICE";
			*sValue_To_Assign = "";
		}
		else
		{
			sCMD_Result = "OK";
			*sValue_To_Assign = pDeviceData_Router->m_sState_get();
		}
	}
	else
	{	
		//get status for all devices
		for(map<int, DeviceData_Router *>::iterator it_device = m_pRouter->DataLayer()->Devices().begin(); 
			it_device != m_pRouter->DataLayer()->Devices().end(); ++it_device)
		{
			DeviceData_Router *pDeviceData_Router = it_device->second;
			int iPK_DeviceCategory = pDeviceData_Router->m_dwPK_DeviceCategory;

			if(iPK_DeviceCategory != DEVICECATEGORY_Lighting_Device_CONST && iPK_DeviceCategory != DEVICECATEGORY_Climate_Device_CONST)
				continue;

			*sValue_To_Assign += StringUtils::ltos(pDeviceData_Router->m_dwPK_Device) + "|" + 
				pDeviceData_Router->m_sState_get() + "\n";
		}
	}
}

//<-dceag-c248-b->

	/** @brief COMMAND: #248 - Get Device Status */
	/** Gets the status for a device */

void General_Info_Plugin::CMD_Get_Device_Status(string &sCMD_Result,Message *pMessage)
//<-dceag-c248-e->
{
	cout << "Need to implement command #248 - Get Device Status" << endl;
}

//<-dceag-c272-b->

	/** @brief COMMAND: #272 - Restart DCERouter */
	/** Causes DCERouter to exit and restart. */
		/** @param #21 Force */

void General_Info_Plugin::CMD_Restart_DCERouter(string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c272-e->
{
	cout << "Need to implement command #272 - Restart DCERouter" << endl;
	cout << "Parm #21 - Force=" << sForce << endl;
}

//<-dceag-c274-b->

	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device, or if negative, this is a PK_PnpQueue and needs to be forwarded to plug and play plugin */
		/** @param #50 Name */
			/** If PK_Room is empty, a new room with this name will be created */
		/** @param #57 PK_Room */
			/** The room */

void General_Info_Plugin::CMD_Set_Room_For_Device(int iPK_Device,string sName,int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c274-e->
{
	cout << "Need to implement command #274 - Set Room For Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #50 - Name=" << sName << endl;
	cout << "Parm #57 - PK_Room=" << iPK_Room << endl;
}

//<-dceag-c322-b->

	/** @brief COMMAND: #322 - Wake Device */
	/** Sends a Wake on LAN to the specified device. */
		/** @param #2 PK_Device */
			/** The device to wake up */

void General_Info_Plugin::CMD_Wake_Device(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c322-e->
{
	cout << "Need to implement command #322 - Wake Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
}

//<-dceag-c323-b->

	/** @brief COMMAND: #323 - Halt Device */
	/** Halts, or suspends, the given device. */
		/** @param #2 PK_Device */
			/** The device to halt */
		/** @param #21 Force */
			/** If Force is not specified this will do a suspend if the device supports suspend/resume, otherwise it will do a halt.  Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot */
		/** @param #47 Mac address */
			/** If PK_Device is not specified (is 0), we'll use the mac address to determine the device id */

void General_Info_Plugin::CMD_Halt_Device(int iPK_Device,string sForce,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c323-e->
{
	cout << "Need to implement command #323 - Halt Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #21 - Force=" << sForce << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
}

//<-dceag-c365-b->

	/** @brief COMMAND: #365 - Get Room Description */
	/** Given a room or device ID, returns the description.  If device ID, also returns the room ID. */
		/** @param #2 PK_Device */
			/** The ID of the device.  Specify this or the room ID. */
		/** @param #9 Text */
			/** The description of the room */
		/** @param #57 PK_Room */
			/** The ID of the room.  Specify this or the device ID. */

void General_Info_Plugin::CMD_Get_Room_Description(int iPK_Device,string *sText,int *iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c365-e->
{
	cout << "Need to implement command #365 - Get Room Description" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #57 - PK_Room=" << iPK_Room << endl;
}

//<-dceag-c371-b->

	/** @brief COMMAND: #371 - Is Daytime */
	/** Returns true or false to indicate if it is daytime (ie between sunrise and sunset) */
		/** @param #119 True/False */
			/** Returns true if it is daytime. */

void General_Info_Plugin::CMD_Is_Daytime(bool *bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c371-e->
{
	cout << "Need to implement command #371 - Is Daytime" << endl;
	cout << "Parm #119 - TrueFalse=" << bTrueFalse << endl;
}

//<-dceag-c395-b->

	/** @brief COMMAND: #395 - Check for updates */
	/** Check all PC's in the system to see if there are available updates on any of them by having all AppServer's run /usr/pluto/bin/Config_Device_Changes.sh */

void General_Info_Plugin::CMD_Check_for_updates(string &sCMD_Result,Message *pMessage)
//<-dceag-c395-e->
{
	cout << "Need to implement command #395 - Check for updates" << endl;
}

//<-dceag-c396-b->

	/** @brief COMMAND: #396 - Check for updates done */
	/** An App Server finished running /usr/pluto/bin/Config_Device_Changes.sh and notifies the g.i. plugin. */
		/** @param #230 Failed */
			/** This is true if the script used to check for updates fails. */

void General_Info_Plugin::CMD_Check_for_updates_done(bool bFailed,string &sCMD_Result,Message *pMessage)
//<-dceag-c396-e->
{
	cout << "Need to implement command #396 - Check for updates done" << endl;
	cout << "Parm #230 - Failed=" << bFailed << endl;
}

//<-dceag-c718-b->

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

void General_Info_Plugin::CMD_Create_Device(int iPK_DeviceTemplate,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,int iPK_DHCPDevice,int iPK_Device_ControlledVia,string sDescription,int iPK_Orbiter,int iPK_Device_Related,int *iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c718-e->
{
	/*
		//TODO: get iPK_DeviceTemplate from iPK_DHCPDevice
	*/

	if( !iPK_DeviceTemplate )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "General_Info_Plugin::CMD_Create_Device Device Template not specified");
		return;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Create_Device template %d mac %s room %d ip %d data %s",
		iPK_DeviceTemplate, sMac_address.c_str(), iPK_Room, sIP_Address.c_str(), sData_String.c_str());
#endif

	DeviceData_Router *pDeviceData_Router = m_pRouter->DataLayer()->CreateDevice(iPK_DeviceTemplate, sDescription, sIP_Address, sMac_address, iPK_Device_ControlledVia);
	pDeviceData_Router->m_dwPK_Room = iPK_Room;

	*iPK_Device = pDeviceData_Router->m_dwPK_Device;

	if(!sData_String.empty())
	{
		string::size_type pos=0;
		while(pos < sData_String.size())
		{
			string sPK_DeviceData = StringUtils::Tokenize(sData_String, "|", pos);
			if(sPK_DeviceData == "E")
			{
				//bEmbedded = true;
			}
			else if(sPK_DeviceData == "P")
			{
				int PK_Command_Input = atoi(StringUtils::Tokenize(sData_String,"|",pos).c_str());
				int PK_Command_Ouput = atoi(StringUtils::Tokenize(sData_String,"|",pos).c_str());
				int PK_Pipe = atoi(StringUtils::Tokenize(sData_String,"|",pos).c_str());
				bool bToChild = atoi(StringUtils::Tokenize(sData_String,"|",pos).c_str())==1;

				//TODO: implement this
				//listCdPipe.push_back(new CdPipe(PK_Command_Input,PK_Command_Ouput,PK_Pipe,bToChild));
			}
			else
			{
				int PK_DeviceData = atoi(sPK_DeviceData.c_str());
				string sValue = StringUtils::Tokenize(sData_String,"|",pos);
				m_pRouter->DataLayer()->SetDeviceData(pDeviceData_Router, PK_DeviceData, sValue);

				if(PK_DeviceData == DEVICEDATA_sPK_Device_Relations_For_Creat_CONST)
				{
					vector<string> vectDevices;
					StringUtils::Tokenize(sValue,"\t",vectDevices);

					//TODO: implement this
					//for(vector<string>::iterator it=vectDevices.begin();it!=vectDevices.end();++it)
					//	CreateRelation(PK_Device,atoi(it->c_str()));
				}
				else if(PK_DeviceData == DEVICEDATA_Description_CONST && !sValue.empty())
				{
					pDeviceData_Router->m_sDescription = sValue;
				}
			}
		}
	}


#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device created %d template %d mac %s room %d ip %d data %s",
			*iPK_Device,iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str());
#endif

	m_pRouter->DataLayer()->Save();

	Message *pMessage_Event = new Message(m_dwPK_Device,DEVICEID_EVENTMANAGER,PRIORITY_NORMAL,MESSAGETYPE_EVENT,EVENT_New_Device_Created_CONST,
		1,EVENTPARAMETER_PK_Device_CONST,StringUtils::itos(*iPK_Device).c_str());
	QueueMessageToRouter(pMessage_Event);
}

//<-dceag-c719-b->

	/** @brief COMMAND: #719 - Delete Device */
	/** Deletes a device */
		/** @param #2 PK_Device */
			/** The device to delete */

void General_Info_Plugin::CMD_Delete_Device(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c719-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Delete_Device device %d", iPK_Device);
#endif

	vector<DeviceData_Router *> vectDevice_Children;
	m_pRouter->DataLayer()->ChildrenDevices(iPK_Device, vectDevice_Children);
	for(vector<DeviceData_Router *>::iterator it_child = vectDevice_Children.begin(); it_child != vectDevice_Children.end(); ++it_child)
	{
		DeviceData_Router *pDeviceData_Router = *it_child;
		CMD_Delete_Device(pDeviceData_Router->m_dwPK_Device);
	}	

	DeviceData_Router *pDeviceData_Router = m_pRouter->DataLayer()->Device(iPK_Device);
	
	if(NULL != pDeviceData_Router)
		pDeviceData_Router->MarkAsDeleted();

	m_pRouter->DataLayer()->Save();
}

//<-dceag-c752-b->

	/** @brief COMMAND: #752 - Check Mounts */
	/** Re-mount all network storage devices */

void General_Info_Plugin::CMD_Check_Mounts(string &sCMD_Result,Message *pMessage)
//<-dceag-c752-e->
{
	cout << "Need to implement command #752 - Check Mounts" << endl;
}

//<-dceag-c765-b->

	/** @brief COMMAND: #765 - Set Device Relations */
	/** Set which devices are related to which */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #103 List PK Device */
			/** The devices it relates to */
		/** @param #205 Reverse */
			/** If true, the device list are the source devices and the PK_Device is the relation */

void General_Info_Plugin::CMD_Set_Device_Relations(int iPK_Device,string sList_PK_Device,bool bReverse,string &sCMD_Result,Message *pMessage)
//<-dceag-c765-e->
{
	cout << "Need to implement command #765 - Set Device Relations" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #103 - List_PK_Device=" << sList_PK_Device << endl;
	cout << "Parm #205 - Reverse=" << bReverse << endl;
}

//<-dceag-c789-b->

	/** @brief COMMAND: #789 - Force Update Packages */
	/** For the system to upgrade */

void General_Info_Plugin::CMD_Force_Update_Packages(string &sCMD_Result,Message *pMessage)
//<-dceag-c789-e->
{
	cout << "Need to implement command #789 - Force Update Packages" << endl;
}

//<-dceag-c791-b->

	/** @brief COMMAND: #791 - Set Enable Status */
	/** This command will enable or disable a device. A reload router will be needed in order to take effect. */
		/** @param #2 PK_Device */
			/** The id of the device to be enabled/disabled */
		/** @param #208 Enable */
			/** If this is true ( = 1), the device will be enabled. Otherwise, the device will be disabled. */

void General_Info_Plugin::CMD_Set_Enable_Status(int iPK_Device,bool bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c791-e->
{
	cout << "Need to implement command #791 - Set Enable Status" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #208 - Enable=" << bEnable << endl;
}

//<-dceag-c800-b->

	/** @brief COMMAND: #800 - InitAVDeviceTemplateSettings */
	/** Save default values in database for a deviceTemplate */
		/** @param #44 PK_DeviceTemplate */
			/** Template id */

void General_Info_Plugin::CMD_InitAVDeviceTemplateSettings(int iPK_DeviceTemplate,string &sCMD_Result,Message *pMessage)
//<-dceag-c800-e->
{
	cout << "Need to implement command #800 - InitAVDeviceTemplateSettings" << endl;
	cout << "Parm #44 - PK_DeviceTemplate=" << iPK_DeviceTemplate << endl;
}

//<-dceag-c802-b->

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

void General_Info_Plugin::CMD_Get_Available_Storage_Device(int iSize,int *iPK_Device,string *sDescription,string *sPath,string &sCMD_Result,Message *pMessage)
//<-dceag-c802-e->
{
	cout << "Need to implement command #802 - Get Available Storage Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #163 - Description=" << sDescription << endl;
	cout << "Parm #219 - Path=" << sPath << endl;
	cout << "Parm #222 - Size=" << iSize << endl;
}

//<-dceag-c803-b->

	/** @brief COMMAND: #803 - Blacklist Internal Disk Drive */
	/** Adds an internal disk drive device to the list of blacklisted drives. */
		/** @param #156 PK_Device_ControlledVia */
			/** The 'computer' device that contains the blacklisted drive. */
		/** @param #223 Block Device */
			/** Linux block device representing the Internal Disk Drive. */

void General_Info_Plugin::CMD_Blacklist_Internal_Disk_Drive(int iPK_Device_ControlledVia,string sBlock_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c803-e->
{
	cout << "Need to implement command #803 - Blacklist Internal Disk Drive" << endl;
	cout << "Parm #156 - PK_Device_ControlledVia=" << iPK_Device_ControlledVia << endl;
	cout << "Parm #223 - Block_Device=" << sBlock_Device << endl;
}

//<-dceag-c808-b->

	/** @brief COMMAND: #808 - Get Unused Serial Ports */
	/** Get the unused serial ports */
		/** @param #2 PK_Device */
			/** The computer to get the ports for */
		/** @param #5 Value To Assign */
			/** A comma delimited list of ports */

void General_Info_Plugin::CMD_Get_Unused_Serial_Ports(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c808-e->
{
	cout << "Need to implement command #808 - Get Unused Serial Ports" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
}

//<-dceag-c813-b->

	/** @brief COMMAND: #813 - Add Software */
	/** Install Software on media director */
		/** @param #2 PK_Device */
			/** The MD to install it on */
		/** @param #119 True/False */
			/** True = add the package, false=remove it */
		/** @param #229 PK_Software */
			/** ID of installing package */

void General_Info_Plugin::CMD_Add_Software(int iPK_Device,bool bTrueFalse,int iPK_Software,string &sCMD_Result,Message *pMessage)
//<-dceag-c813-e->
{
	cout << "Need to implement command #813 - Add Software" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #119 - TrueFalse=" << bTrueFalse << endl;
	cout << "Parm #229 - PK_Software=" << iPK_Software << endl;
}

//<-dceag-c833-b->

	/** @brief COMMAND: #833 - Get User Name */
	/** Given a PK_User, get the users name */
		/** @param #5 Value To Assign */
			/** The user's name */
		/** @param #17 PK_Users */
			/** The user id */

void General_Info_Plugin::CMD_Get_User_Name(int iPK_Users,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c833-e->
{
	cout << "Need to implement command #833 - Get User Name" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #17 - PK_Users=" << iPK_Users << endl;
}

//<-dceag-c835-b->

	/** @brief COMMAND: #835 - Get Network Devices Shares */
	/** Get the list with shares for all network devices from this installation. */
		/** @param #244 Custom Response */
			/** Contains the following info:

device id, device ip, share name, user name, password, free space.

Delimiter: '\n' */

void General_Info_Plugin::CMD_Get_Network_Devices_Shares(char **pCustom_Response,int *iCustom_Response_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c835-e->
{
	cout << "Need to implement command #835 - Get Network Devices Shares" << endl;
	cout << "Parm #244 - Custom_Response  (data value)" << endl;
}

//<-dceag-c843-b->

	/** @brief COMMAND: #843 - RemoteAssistance_SetStatus */
	/** Change the state of Remote Assistance */
		/** @param #208 Enable */
			/** Desired state of Remote Assistance */

void General_Info_Plugin::CMD_RemoteAssistance_SetStatus(bool bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c843-e->
{
	cout << "Need to implement command #843 - RemoteAssistance_SetStatus" << endl;
	cout << "Parm #208 - Enable=" << bEnable << endl;
}

//<-dceag-c844-b->

	/** @brief COMMAND: #844 - RemoteAssistance_GetStatus */
	/** Get the state of Remote Assistance */
		/** @param #208 Enable */
			/** The state of Remote Assistance */

void General_Info_Plugin::CMD_RemoteAssistance_GetStatus(bool *bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c844-e->
{
	cout << "Need to implement command #844 - RemoteAssistance_GetStatus" << endl;
	cout << "Parm #208 - Enable=" << bEnable << endl;
}

//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void General_Info_Plugin::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
	cout << "Need to implement command #882 - Abort Task" << endl;
	cout << "Parm #248 - Parameter_ID=" << iParameter_ID << endl;
}

//<-dceag-c915-b->

	/** @brief COMMAND: #915 - Enable Device */
	/** Called when instead of creating a device, we re-enabled one that had been disabled and need to call the postcreateoptions again */
		/** @param #2 PK_Device */
			/** The device that was enabled */
		/** @param #198 PK_Orbiter */
			/** The Orbiter that did it, if any */

void General_Info_Plugin::CMD_Enable_Device(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c915-e->
{
	cout << "Need to implement command #915 - Enable Device" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #198 - PK_Orbiter=" << iPK_Orbiter << endl;
}

//<-dceag-c934-b->

	/** @brief COMMAND: #934 - Get Home Symlink */
	/** Get home symlink */
		/** @param #219 Path */
			/** The physical path like /mnt/device/<deviceid> */
		/** @param #275 Symlink */
			/** The symlink like /home/public/data ... */

void General_Info_Plugin::CMD_Get_Home_Symlink(string sPath,string *sSymlink,string &sCMD_Result,Message *pMessage)
//<-dceag-c934-e->
{
	cout << "Need to implement command #934 - Get Home Symlink" << endl;
	cout << "Parm #219 - Path=" << sPath << endl;
	cout << "Parm #275 - Symlink=" << sSymlink << endl;
}
//----------------------------------------------------------------------------------------------
bool General_Info_Plugin::ReportingChildDevices( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string sError = pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];
	if( sError.size() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ReportingChildDevices Device %d failed to report its children: %s",
			pMessage->m_dwPK_Device_From,sError.c_str());
		return false;
	}

	string sChildren = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];
	ChildDeviceProcessing *pChildDeviceProcessing = new ChildDeviceProcessing(pMessage->m_dwPK_Device_From,sChildren);  // Do this in a separate thread since create_device is slow and we don't want to block teh socket
	m_pAlarmManager->AddRelativeAlarm(1,this,PROCESS_CHILD_DEVICES,(void *) pChildDeviceProcessing);
	return false;
}
//----------------------------------------------------------------------------------------------
void General_Info_Plugin::AlarmCallback(int id, void* param)
{
	if( id==PROCESS_CHILD_DEVICES )
		ReportingChildDevices_Offline(param);
	else if( id==PROCESS_DELAYED_EXECUTE_SCENARIOS )
		ProcessDelayedExecuteScenarios();
}
//----------------------------------------------------------------------------------------------
void General_Info_Plugin::ReportingChildDevices_Offline( void *pVoid )
{
	ChildDeviceProcessing *pChildDeviceProcessing = (ChildDeviceProcessing *) pVoid;

	map<int,bool> mapCurrentChildren;
	vector<string> vectLines;
	StringUtils::Tokenize(pChildDeviceProcessing->m_sChildren,"\n",vectLines);

	int iNewDevices=0,iRemovedDevices=0;

	for(vector<string>::iterator it = vectLines.begin(); it != vectLines.end(); ++it)
	{
		string sLine = *it;
		bool bNewDevice;
		// This will add the child device if it doesn't exist
		DeviceData_Router *pDevice_Child = ProcessChildDevice(pChildDeviceProcessing->m_nPK_Device, sLine, bNewDevice);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ReportingChildDevices_Offline line %s child %d new %d", sLine.c_str(), pDevice_Child ? pDevice_Child->m_dwPK_Device : -1, (int) bNewDevice);
		if(NULL != pDevice_Child)
			mapCurrentChildren[pDevice_Child->m_dwPK_Device] = true;
		if( bNewDevice )
			iNewDevices++;
	}

	// See if any child devices have since disappeared
    vector<DeviceData_Router *> vectDevice_Children;
	m_pRouter->DataLayer()->ChildrenDevices(pChildDeviceProcessing->m_nPK_Device, vectDevice_Children);
	for(vector<DeviceData_Router *>::iterator it_child = vectDevice_Children.begin(); it_child != vectDevice_Children.end(); ++it_child)
	{
		DeviceData_Router *pDeviceData_Router = *it_child;

		if(NULL != pDeviceData_Router->m_pDevice_RouteTo)
		{
			// Skip embedded children, iterate through their children
			vector<DeviceData_Router *> vectDevice_Children_Lev2;
			m_pRouter->DataLayer()->ChildrenDevices(pDeviceData_Router->m_dwPK_Device, vectDevice_Children_Lev2);

			for(vector<DeviceData_Router *>::iterator it_child2 = vectDevice_Children.begin(); it_child2 != vectDevice_Children.end(); ++it_child2)
			{
				DeviceData_Router *pDevice_Child = *it_child2;

                if(!mapCurrentChildren[pDevice_Child->m_dwPK_Device])
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ReportingChildDevices removing dead embedded device %d %s",
						pDevice_Child->m_dwPK_Device, pDevice_Child->m_sDescription.c_str());
					CMD_Delete_Device(pDevice_Child->m_dwPK_Device);
					iRemovedDevices++;
				}
				continue; // Don't delete the embedded device
			}
		}

		if(!mapCurrentChildren[pDeviceData_Router->m_dwPK_Device])
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ReportingChildDevices removing dead device %d %s",
				pDeviceData_Router->m_dwPK_Device, pDeviceData_Router->m_sDescription.c_str());
			CMD_Delete_Device(pDeviceData_Router->m_dwPK_Device);
			iRemovedDevices++;
		}
	}

	m_pRouter->DataLayer()->Save();

	delete pChildDeviceProcessing;

	if( iNewDevices || iRemovedDevices )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ReportingChildDevices reloading because of %d new and %d removed devices", iNewDevices, iRemovedDevices);
		Message *pMessage = new Message( m_dwPK_Device, DEVICEID_DCEROUTER, PRIORITY_NORMAL, MESSAGETYPE_SYSCOMMAND, SYSCOMMAND_RELOAD, 0 );
		QueueMessageToRouter( pMessage );
	}
}
//----------------------------------------------------------------------------------------------
// It will be like this:
// [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \t [PK_DeviceData] \t [Value] ... \n

DeviceData_Router *General_Info_Plugin::ProcessChildDevice(int nPK_Device, string sLine, bool &bNewDevice)
{
	bNewDevice = false;

	string::size_type pos=0;
	string sInternalID = StringUtils::Tokenize(sLine,"\t",pos);
	string sDescription = StringUtils::Tokenize(sLine,"\t",pos);
	string sRoomName = StringUtils::Tokenize(sLine,"\t",pos);
	int PK_DeviceTemplate = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
	string sPK_FloorplanObjectType = StringUtils::Tokenize(sLine,"\t",pos);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ProcessChildDevice: pos=%d, %s", pos, sLine.c_str());

	DeviceTemplate_Data *pDeviceTemplate_Data = m_pRouter->DataLayer()->DeviceTemplate(PK_DeviceTemplate);
	if(NULL == pDeviceTemplate_Data)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "General_Info_Plugin::ProcessChildDevice Line %s has unknown PK_DeviceTemplate %d", sLine.c_str(), PK_DeviceTemplate);
		return NULL;
	}

	// Find the child device with this internal id
	DeviceData_Router *pDeviceData_Router = m_pRouter->DataLayer()->ChildMatchingDeviceData(nPK_Device, DEVICEDATA_PortChannel_Number_CONST, StringUtils::SQLEscape(sInternalID));
	if(NULL != pDeviceData_Router)
	{
		if(pDeviceData_Router->m_dwPK_DeviceTemplate == PK_DeviceTemplate)
			return pDeviceData_Router;
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ProcessChildDevice (Reporting) line %s device %d deleting temp %d!=%d", 
				sLine.c_str(), pDeviceData_Router->m_dwPK_Device, pDeviceData_Router->m_dwPK_DeviceTemplate, PK_DeviceTemplate);
			CMD_Delete_Device(pDeviceData_Router->m_dwPK_Device);
		}
	}
	
	// Create it since it doesn't exist
	int iPK_Device = 0;
	bNewDevice = true;

	CMD_Create_Device(PK_DeviceTemplate,"",0,"",
		StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalID,
		0, nPK_Device,"",0,0, &iPK_Device);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ProcessChildDevice created %d from %s", nPK_Device, sLine.c_str());

	pDeviceData_Router = m_pRouter->DataLayer()->Device(iPK_Device);
	if(NULL == pDeviceData_Router)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ProcessChildDevice failed to create child %d",iPK_Device);
		return NULL;
	}

	// Don't reset the description if it's already there, the user may have overridden the default name
	if(pDeviceData_Router->m_sDescription.empty())
	{
		if(!sDescription.empty())
			pDeviceData_Router->m_sDescription = sDescription;
		else
		{
			DeviceTemplate_Data *pDeviceTemplate_Data = m_pRouter->DataLayer()->DeviceTemplate(pDeviceData_Router->m_dwPK_DeviceTemplate);
			if(pDeviceTemplate_Data)
				pDeviceData_Router->m_sDescription = pDeviceTemplate_Data->Description();
			else
				pDeviceData_Router->m_sDescription = sInternalID;
		}
	}
	else if(bNewDevice)
	{
		string sOldDescription = pDeviceData_Router->m_sDescription;

		if(!sDescription.empty())
			pDeviceData_Router->m_sDescription = sOldDescription + " " + sDescription;
		else
			pDeviceData_Router->m_sDescription = sOldDescription + " " + sInternalID;
	}

/*
	//TODO: implement this in DataLayer

	Room_Data *pRoom_Data = m_pRouter->Room(StringUtils::SQLEscape(sRoomName));
	if(NULL != pRoom_Data)
	{
		pDeviceData_Router->m_dwPK_Room = pRoom_Data->ID();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %d %s in unknown room %s",
			pDeviceData_Router->m_dwPK_Device, sDescription.c_str(), sRoomName.c_str());
	}
*/

	pDeviceData_Router->m_dwPK_DeviceTemplate = PK_DeviceTemplate;

	if(atoi(sPK_FloorplanObjectType.c_str()) > 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ProcessChildDevice setting sPK_FloorplanObjectType to %s for %d",
			sPK_FloorplanObjectType.c_str(), pDeviceData_Router->m_dwPK_Device);

		m_pRouter->DataLayer()->SetDeviceData(pDeviceData_Router, DEVICEDATA_PK_FloorplanObjectType_CONST, sPK_FloorplanObjectType);
	}

	while(pos < sLine.size())
	{
		int PK_DeviceData = atoi(StringUtils::Tokenize(sLine, "\t", pos).c_str());
		string sValue = StringUtils::Tokenize(sLine, "\t", pos);
		if(0 != PK_DeviceData)
			m_pRouter->DataLayer()->SetDeviceData(pDeviceData_Router, PK_DeviceData, sValue);
	}

	return pDeviceData_Router;
}
//----------------------------------------------------------------------------------------------
//<-dceag-sample-b->!
//<-dceag-sample-e->
//----------------------------------------------------------------------------------------------
//<-dceag-c956-b->

	/** @brief COMMAND: #956 - Get Devices To Start */
	/** Get the list with devices to start. */
		/** @param #2 PK_Device */
			/** The parent device */
		/** @param #5 Value To Assign */
			/** A pipe delimited list like this: DeviceID1|TemplateName1|CommandLine1\nDeviceID2|DeviceTemplateName2|CommandLine2 etc */

void General_Info_Plugin::CMD_Get_Devices_To_Start(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c956-e->
{
	PLUTO_SAFETY_LOCK(dm, m_pRouter->DataLayer()->Mutex());

	std::vector<DeviceData_Router *> vectDevices;
	m_pRouter->DataLayer()->ChildrenDevices(iPK_Device, vectDevices, true);

	for(std::vector<DeviceData_Router *>::const_iterator it = vectDevices.begin(); it != vectDevices.end(); ++it)
	{
		DeviceData_Router *pDeviceData_Router = *it;
		DeviceTemplate_Data *pDeviceTemplate_Data = m_pRouter->DataLayer()->DeviceTemplate(pDeviceData_Router->m_dwPK_DeviceTemplate);

		string sTemplateName;
		if(NULL != pDeviceTemplate_Data)
			sTemplateName = pDeviceTemplate_Data->Description();

		*sValue_To_Assign += StringUtils::ltos(pDeviceData_Router->m_dwPK_Device) + "|" + 
			sTemplateName + "|" + 
			pDeviceData_Router->m_sCommandLine + "\n";
	}
}
//----------------------------------------------------------------------------------------------
bool General_Info_Plugin::DeviceDetected( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string sDeviceData = pMessage->m_mapParameters[EVENTPARAMETER_DeviceData_CONST];
	string sIPAddress = pMessage->m_mapParameters[EVENTPARAMETER_IP_Address_CONST];
	string sMacAddress = StringUtils::ToUpper(pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST]);
	int nPK_DeviceTemplate = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_DeviceTemplate_CONST].c_str());

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device detected: template id %d, ip %s, mac %s, device data %s",
		nPK_DeviceTemplate, sIPAddress.c_str(), sMacAddress.c_str(), sDeviceData.c_str());

	bool bDeviceExists = false;

	list<DeviceData_Router *> listDevices;
	m_pRouter->DataLayer()->DevicesByTemplate(nPK_DeviceTemplate, listDevices);

	for(list<DeviceData_Router *>::iterator it = listDevices.begin(), end = listDevices.end(); it != end; ++it)
	{
		DeviceData_Router *pDeviceData_Router = *it;

		if( pDeviceData_Router->m_sMacAddress == sMacAddress)
		{
			bDeviceExists = true;

			if(pDeviceData_Router->m_sIPAddress == sIPAddress)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device already created, nothing is changed");
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device already created, need to update the ip addresss");
				pDeviceData_Router->m_sIPAddress = sIPAddress;
				m_pRouter->DataLayer()->Save();
			}
			break;
		}
	}

	if(!bDeviceExists)
	{
		DeviceTemplate_Data* pDeviceTemplate_Data = m_pRouter->DataLayer()->DeviceTemplate(nPK_DeviceTemplate);

		string sDeviceDescription;

		if(NULL != pDeviceTemplate_Data)
			sDeviceDescription = pDeviceTemplate_Data->Description();
		else
		{
			sDeviceDescription = "Unknown device";
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unknown device template %d", nPK_DeviceTemplate);
		}

		std::list<DeviceData_Router *> listRouterDevices;
		m_pRouter->DataLayer()->DevicesByTemplate(DEVICETEMPLATE_DCERouter_CONST, listRouterDevices);
		int nRoomID = 0;

		if(!listRouterDevices.empty())
		{
			DeviceData_Router *pRouter = *(listRouterDevices.begin());
			nRoomID = pRouter->m_dwPK_Room;
		}

		string sCMD_Result;
		int nPK_Device = 0;
		CMD_Create_Device(
			nPK_DeviceTemplate, 
			sMacAddress, 
			nRoomID, 
			sIPAddress, 
			sDeviceData, 
			0, //iPK_DHCPDevice
			0, //iPK_Device_ControlledVia
			"", //sDescription
			0, //iPK_Orbiter
			0, //iPK_Device_Related
			&nPK_Device,
			sCMD_Result,
			pMessage
		);

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device created: %d", nPK_Device);
		m_pRouter->DataLayer()->Save();

		Message *pMessage = new Message( m_dwPK_Device, DEVICEID_DCEROUTER, PRIORITY_NORMAL, MESSAGETYPE_SYSCOMMAND, SYSCOMMAND_RELOAD, 0 );
		QueueMessageToRouter( pMessage );
	}

	return false;
}
//----------------------------------------------------------------------------------------------
bool General_Info_Plugin::DeviceRemoved( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string sDeviceData = pMessage->m_mapParameters[EVENTPARAMETER_DeviceData_CONST];
	string sIPAddress = pMessage->m_mapParameters[EVENTPARAMETER_IP_Address_CONST];
	string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
	int nPK_DeviceTemplate = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_DeviceTemplate_CONST].c_str());

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device removed: template id %d, ip %s, mac %s, device data %s",
		nPK_DeviceTemplate, sIPAddress.c_str(), sMacAddress.c_str(), sDeviceData.c_str());

	//TODO: do we need this now ?

	return false;
}
//----------------------------------------------------------------------------------------------

//<-dceag-c957-b->

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

void General_Info_Plugin::CMD_Update_Device(int iPK_Device,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,string sDescription,string &sCMD_Result,Message *pMessage)
//<-dceag-c957-e->
{
	DeviceData_Router *pDeviceData_Router = m_pRouter->DataLayer()->Device(iPK_Device);

	if(NULL == pDeviceData_Router)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to update device %d, it doesn't exist", iPK_Device);
		sCMD_Result = "ERROR: Device doesn't exist";
		return;
	}

	PLUTO_SAFETY_LOCK(dm, m_pRouter->DataLayer()->Mutex());

	pDeviceData_Router->m_sDescription = sDescription;
	pDeviceData_Router->m_sIPAddress = sIP_Address;
	pDeviceData_Router->m_sMacAddress = sMac_address;
	pDeviceData_Router->m_dwPK_Room = iPK_Room;

	vector<string> vectStrings;
	StringUtils::Tokenize(sData_String, "|", vectStrings);
	for(vector<string>::iterator it = vectStrings.begin(), end = vectStrings.end(); it != end; ++it)
	{
		string sKey = *it;
		int nKey = atoi(sKey.c_str());

		if( ++it == end )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "General_Info_Plugin::CMD_Update_Device parms %s are malformed", sData_String.c_str());
			break;
		}
		else
		{
			string sValue = *it;
			pDeviceData_Router->ReplaceParameter(nKey, sValue);

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Updating device data %d with value %s",
				nKey, sValue.c_str());
		}
	}

	m_pRouter->DataLayer()->Save();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Update_Device saved data for device %d %s in room %d",
		pDeviceData_Router->m_dwPK_Device, pDeviceData_Router->m_sDescription.c_str(), pDeviceData_Router->m_dwPK_Room);

}

//<-dceag-c370-b->

	/** @brief COMMAND: #370 - Execute Command Group */
	/** Execute a command group */
		/** @param #9 Text */
			/** Instead of the command group, it can be put here in the format: PK_Device,PK_DeviceGroup,PK_Command,Delay,CancelIfOther,IsTemporary,"PK_CommandParameter","Description"....\n

where the items in " have escaped " so they can embed , and \n characters */
		/** @param #28 PK_CommandGroup */
			/** The command group to execute */

void General_Info_Plugin::CMD_Execute_Command_Group(string sText,int iPK_CommandGroup,string &sCMD_Result,Message *pMessage)
//<-dceag-c370-e->
{
	if(iPK_CommandGroup && NULL != m_pRouter->DataLayer())
	{
		Scene_Data *pScene_Data =  m_pRouter->DataLayer()->Scene(iPK_CommandGroup);
		if(NULL != pScene_Data)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Execute_Command_Group executing %d with %d commands", iPK_CommandGroup, (int) pScene_Data->Commands().size() );
			ExecuteCommandData(&(pScene_Data->Commands()),pMessage->m_dwPK_Device_From);
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "General_Info_Plugin::CMD_Execute_Command_Group Unexisting scene: %d", iPK_CommandGroup);
	}
	else if( sText.empty()==false )
	{
		Scene_Data scene_Data;

		int iIndex=0;
		string::size_type pos=0;
		while(pos<sText.size())
		{
			Command_Data aCommand_data;
			aCommand_data.Device_To(atoi(StringUtils::Tokenize( sText, ",", pos ).c_str()));
			int PK_DeviceGroup = atoi(StringUtils::Tokenize( sText, ",", pos ).c_str());
			aCommand_data.PK_Command(atoi(StringUtils::Tokenize( sText, ",", pos ).c_str()));
			aCommand_data.Delay(atoi(StringUtils::Tokenize( sText, ",", pos ).c_str()));
			aCommand_data.CancelIfOther(atoi(StringUtils::Tokenize( sText, ",", pos ).c_str()));
			aCommand_data.IsTemporary(atoi(StringUtils::Tokenize( sText, ",\n", pos ).c_str()));  // This may be the end too
aCommand_data.CancelIfOther(1);
aCommand_data.IsTemporary(1);
			if( sText[pos-1]!='\n' )
				ParseCommandParameters(aCommand_data.Params(), sText, pos);
			if( aCommand_data.Device_To()==0 || aCommand_data.PK_Command()==0 )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "General_Info_Plugin::CMD_Execute_Command_Group scene: %d has empty data", iPK_CommandGroup);
			else
				scene_Data.Commands()[iIndex++] = aCommand_data;
			while( (sText[pos]=='\n' || sText[pos]=='\r') && pos<sText.size() )
				++pos;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Execute_Command_Group executing temp %d commands", (int) scene_Data.Commands().size() );
		ExecuteCommandData(&(scene_Data.Commands()),pMessage->m_dwPK_Device_From);
	}
}


void General_Info_Plugin::ParseCommandParameters(std::map<int, string>& mapParams, string &sText,string::size_type &pos)
{
	while(true)
	{
		int PK_CommandParameter = atoi(StringUtils::Tokenize( sText, ",", pos ).c_str());
		string sValue;
		if( sText[pos]=='"' )
		{
			size_t size = sText.size();
			string::size_type pos_start = ++pos;
			while( pos++<size )
			{
				if( sText[pos]=='"' && sText[pos-1]!='\\' )
				{
					sValue = sText.substr(pos_start, pos-pos_start);
					StringUtils::Replace(&sValue,"\\\"", "\""); // Replace any escaped "
					pos++;
					if( pos<sText.size() && (sText[pos]==',' || sText[pos]=='\n') )
						pos++; // Skip the comma or \n
					break;
				}
			}
		}
		else
			sValue = StringUtils::Tokenize( sText, "\n,", pos );

		mapParams[PK_CommandParameter]=sValue;
		if( sText[pos-1]=='\n' || pos>=sText.size() )
			return;
	}
}

void General_Info_Plugin::ExecuteCommandData(map<int, Command_Data> *mapCommands,int PK_Device_From)
{
	PLUTO_SAFETY_LOCK(dm, m_pRouter->DataLayer()->Mutex());
	for(std::map<int, Command_Data>::iterator it = mapCommands->begin(); it != mapCommands->end(); ++it)
	{
		Command_Data *pCommand_Data = &(it->second);

		if( pCommand_Data->Delay() )
		{
			Command_Data_CallBack *pCommand_Data_CallBack = new Command_Data_CallBack(pCommand_Data,time(NULL) + pCommand_Data->Delay(),PK_Device_From);
			List_Command_Data_CallBack *pList_Command_Data_CallBack = m_mapList_Command_Data_CallBack[ pCommand_Data->Device_To() ];
			if( !pList_Command_Data_CallBack )
			{
				pList_Command_Data_CallBack = new List_Command_Data_CallBack();
				m_mapList_Command_Data_CallBack[ pCommand_Data->Device_To() ] = pList_Command_Data_CallBack;
			}
			pList_Command_Data_CallBack->push_back( pCommand_Data_CallBack );
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ExecuteCommandGroup delay %p/%p", pCommand_Data_CallBack, pCommand_Data_CallBack->m_pCommand_Data);
		}
		else
			ExecuteCommandData(pCommand_Data,PK_Device_From);
	}
	SetNextAlarm();
}

void General_Info_Plugin::SetNextAlarm()
{
	time_t tLowest=0;
	bool bCancellable=false;
	PLUTO_SAFETY_LOCK(dm, m_pRouter->DataLayer()->Mutex());
	for(map<int,List_Command_Data_CallBack *>::iterator it=m_mapList_Command_Data_CallBack.begin();
		it!=m_mapList_Command_Data_CallBack.end();)
	{
		List_Command_Data_CallBack *pList_Command_Data_CallBack = it->second;
		if( pList_Command_Data_CallBack->empty() )
			m_mapList_Command_Data_CallBack.erase(it++);
		else
		{
			for(List_Command_Data_CallBack::iterator itL=pList_Command_Data_CallBack->begin();itL!=pList_Command_Data_CallBack->end();++itL)
			{
				Command_Data_CallBack *pCommand_Data_CallBack = *itL;
				if( pCommand_Data_CallBack->m_pCommand_Data->CancelIfOther() )
					bCancellable=true;
				if( tLowest==0 || tLowest>pCommand_Data_CallBack->m_tTime )
					tLowest = pCommand_Data_CallBack->m_tTime;
			}
			++it;
		}
	}

	m_pAlarmManager->CancelAlarmByType(PROCESS_DELAYED_EXECUTE_SCENARIOS);
	if( tLowest!=0 )
		m_pAlarmManager->AddAbsoluteAlarm(tLowest, this, PROCESS_DELAYED_EXECUTE_SCENARIOS, NULL);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::SetNextAlarm %d (%d) seconds cancellable %d callback %p", time(NULL)-tLowest, tLowest, (int) bCancellable, m_pCallBack_ForExecuteScenarios);

	if( bCancellable && m_pCallBack_ForExecuteScenarios==NULL)
		RegisterAllInterceptor();
	else if( bCancellable==false && m_pCallBack_ForExecuteScenarios!=NULL )
		UnRegisterAllInterceptor();
}

void General_Info_Plugin::RegisterAllInterceptor()
{
	PLUTO_SAFETY_LOCK(dm, m_pRouter->DataLayer()->Mutex());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::RegisterAllInterceptor %p", m_pCallBack_ForExecuteScenarios);
	if( !m_pCallBack_ForExecuteScenarios )
		RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::InterceptAllCommands ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, 0, true, &m_pCallBack_ForExecuteScenarios );
}

void General_Info_Plugin::UnRegisterAllInterceptor()
{
	PLUTO_SAFETY_LOCK(dm, m_pRouter->DataLayer()->Mutex());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::UnRegisterAllInterceptor %p", m_pCallBack_ForExecuteScenarios);
	if( m_pCallBack_ForExecuteScenarios )
		m_pRouter->UnRegisterMsgInterceptor(m_pCallBack_ForExecuteScenarios);
	m_pCallBack_ForExecuteScenarios=NULL;
}

bool General_Info_Plugin::InterceptAllCommands( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( pMessage->m_mapParameters.find(COMMANDPARAMETER_Already_processed_CONST)!=pMessage->m_mapParameters.end() )
		return false; // This originated from us anyway

	PLUTO_SAFETY_LOCK(dm, m_pRouter->DataLayer()->Mutex());
	map<int,List_Command_Data_CallBack *>::iterator it=m_mapList_Command_Data_CallBack.find( pMessage->m_dwPK_Device_To );
	if( it!=m_mapList_Command_Data_CallBack.end() )
	{
		List_Command_Data_CallBack *pList_Command_Data_CallBack = it->second;
		for(List_Command_Data_CallBack::iterator itL=pList_Command_Data_CallBack->begin();itL!=pList_Command_Data_CallBack->end();)
		{
			Command_Data_CallBack *pCommand_Data_CallBack = *itL;
			if( pCommand_Data_CallBack->m_pCommand_Data->CancelIfOther() )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::InterceptAllCommands canceling %p/%p", pCommand_Data_CallBack,pCommand_Data_CallBack->m_pCommand_Data );
				pList_Command_Data_CallBack->erase(itL++);
				delete pCommand_Data_CallBack;
			}
			else
				++itL;
		}
	}
	return false;
}

void General_Info_Plugin::ProcessDelayedExecuteScenarios()
{
	time_t tNow = time(NULL);
	PLUTO_SAFETY_LOCK(dm, m_pRouter->DataLayer()->Mutex());
	for(map<int,List_Command_Data_CallBack *>::iterator it=m_mapList_Command_Data_CallBack.begin();
		it!=m_mapList_Command_Data_CallBack.end();++it)
	{
		List_Command_Data_CallBack *pList_Command_Data_CallBack = it->second;
		for(List_Command_Data_CallBack::iterator itL=pList_Command_Data_CallBack->begin();itL!=pList_Command_Data_CallBack->end();)
		{
			Command_Data_CallBack *pCommand_Data_CallBack = *itL;
			if( pCommand_Data_CallBack->m_tTime<=tNow )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ProcessDelayedExecuteScenarios executing %p/%p", pCommand_Data_CallBack,pCommand_Data_CallBack->m_pCommand_Data );

				ExecuteCommandData(pCommand_Data_CallBack->m_pCommand_Data,pCommand_Data_CallBack->m_PK_Device_From);
				pList_Command_Data_CallBack->erase(itL++);
				delete pCommand_Data_CallBack;
			}
			else
				++itL;
		}
	}

	SetNextAlarm();
}

void General_Info_Plugin::ExecuteCommandData(Command_Data *pCommand_Data,int PK_Device_From)
{
	Message *pMessage = new Message();
	pMessage->m_dwPK_Device_From = PK_Device_From ? PK_Device_From : m_dwPK_Device;
	pMessage->m_dwMessage_Type = MESSAGETYPE_COMMAND;
	pMessage->m_dwID = pCommand_Data->PK_Command();
	pMessage->m_dwPK_Device_To = pCommand_Data->Device_To();
	pMessage->m_mapParameters[COMMANDPARAMETER_Is_Temporary_CONST] = (pCommand_Data->IsTemporary()==0 ? "1" : "0");
	pMessage->m_mapParameters[COMMANDPARAMETER_Already_processed_CONST] = "1";

	if(pMessage->m_dwPK_Device_To == DEVICETEMPLATE_This_Orbiter_CONST && PK_Device_From )
		pMessage->m_dwPK_Device_To = PK_Device_From;

	for(std::map<int, string>::iterator it_param = pCommand_Data->Params().begin(); 
		it_param != pCommand_Data->Params().end(); ++it_param)
	{
		pMessage->m_mapParameters[it_param->first] = it_param->second;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::ExecuteCommandGroup Processing message from %d to %d id %d", 
		pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To, pMessage->m_dwID);

	QueueMessageToRouter(pMessage);
}

//<-dceag-c969-b->

	/** @brief COMMAND: #969 - Restore To NonTemp State */
	/** Restore a device to the state in State_NonTemporary, so that a temporary change can be reverted */
		/** @param #2 PK_Device */
			/** The device to restore */

void General_Info_Plugin::CMD_Restore_To_NonTemp_State(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c969-e->
{
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if( pDevice==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "General_Info_Plugin::CMD_Restore_To_NonTemp_State Device %d is invalid", iPK_Device);
		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Restore_To_NonTemp_State Device %d was %s", iPK_Device, pDevice->m_sState_NonTemporary_get().c_str());
	if( pDevice->m_sState_NonTemporary_get()=="OFF" )
		QueueMessageToRouter( new Message( m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_Off_CONST,0 ) );
	else if( pDevice->m_sState_NonTemporary_get()=="ON" )
		QueueMessageToRouter( new Message( m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST,0 ) );
	else
	{
		Command_Impl *pCommand_Impl = NULL;
		if( pDevice->WithinCategory(DEVICECATEGORY_Climate_Device_CONST) )
			pCommand_Impl = m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Climate_Plugin_CONST);
		else if( pDevice->WithinCategory(DEVICECATEGORY_Security_Device_CONST) )
			pCommand_Impl = m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Security_Plugin_CONST);
		else if( pDevice->WithinCategory(DEVICECATEGORY_Lighting_Device_CONST) )
			pCommand_Impl = m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Lighting_Plugin_CONST);

		if( pCommand_Impl==NULL )
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Restore_To_NonTemp_State Device %d has no plugin", iPK_Device);
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Restore_To_NonTemp_State Device %d forwarding to %d", iPK_Device, pCommand_Impl->m_dwPK_Device);
			QueueMessageToRouter( new Message( m_dwPK_Device,pCommand_Impl->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Restore_To_NonTemp_State_CONST, 1,
				COMMANDPARAMETER_PK_Device_CONST, StringUtils::itos(pCommand_Impl->m_dwPK_Device).c_str()) );
		}
	}
}
