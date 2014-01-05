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
#include "agocontrol_Bridge.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include <stdio.h>



using namespace qpid::messaging;
using namespace qpid::types;
using namespace DCE;
using namespace std;

extern "C" void *start( void* );
void *start( void *p ) {
	agocontrol_Bridge *base = static_cast<agocontrol_Bridge*>(p);
	base->receiveFunction();
	return NULL;
}


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
agocontrol_Bridge::agocontrol_Bridge(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: agocontrol_Bridge_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
agocontrol_Bridge::agocontrol_Bridge(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: agocontrol_Bridge_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
agocontrol_Bridge::~agocontrol_Bridge()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool agocontrol_Bridge::GetConfig()
{
	if( !agocontrol_Bridge_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	Variant::Map connectionOptions;
	connectionOptions["username"] = DATA_Get_Username();
	connectionOptions["password"] = DATA_Get_Password();
	try {
		agoConnection = Connection(DATA_Get_TCP_Address(), connectionOptions);
		agoConnection.open();
		agoSession = agoConnection.createSession();
		agoSender = agoSession.createSender("agocontrol; {create: always, node: {type: topic}}");
		agoReceiver = agoSession.createReceiver("agocontrol; {create: always, node: {type: topic}}");
	} catch(const std::exception& error) {
                std::cerr << error.what() << std::endl;
                agoConnection.close();
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not startup");
                return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Starting receive thread");
	static pthread_t receiveThread;
	pthread_create(&receiveThread, NULL, start, (void*)this);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool agocontrol_Bridge::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
agocontrol_Bridge_Command *Create_agocontrol_Bridge(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new agocontrol_Bridge(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void agocontrol_Bridge::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	qpid::messaging::Message command;
	Variant::Map content;

	string portChannel = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	content["uuid"] = portChannel;

	sCMD_Result = "OK";

	switch (pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"ON RECEIVED FOR CHILD %s", portChannel.c_str());
			content["command"] = "on";
			break;
		case COMMAND_Generic_Off_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"OFF RECEIVED FOR CHILD %s", portChannel.c_str());
			content["command"] = "off";
			break;
		case COMMAND_Set_Level_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"SET LEVEL RECEIVED FOR CHILD %s", portChannel.c_str());
			content["command"] = "setlevel";
			content["level"] = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()); 
			break;
		case COMMAND_Set_Temperature_CONST:
			content["command"] = "settemperature";
			content["temperature"] = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET TEMPERATURE RECEIVED FOR CHILD %s",portChannel.c_str());
			break;
			;;
		case COMMAND_Set_Fan_CONST:
			content["command"] = "setthermostatfanmode";
			int fan = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str());
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"SET FAN RECEIVED FOR CHILD %s, level: %d",portChannel.c_str(),fan);
			if (fan == 1) {
				content["mode"] = "onhigh";
			} else {
				content["mode"] = "autohigh";
			}
			break;
			;;
	}
	encode(content, command);
	agoSender.send(command);
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void agocontrol_Bridge::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

DeviceData_Impl *agocontrol_Bridge::InternalIDToDevice(string sInternalID) {
        DeviceData_Impl *pChildDevice = NULL;

	if (sInternalID == "") return NULL;

        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
	{
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
		        string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
			// check if child exists
			if ( tmp_node_id.compare(sInternalID) == 0) {
			        return pChildDevice;
			}

			// iterate over embedded interfaces
			DeviceData_Impl *pChildDevice1 = NULL;
			for( VectDeviceData_Impl::const_iterator it1 = pChildDevice->m_vectDeviceData_Impl_Children.begin();
				it1 != pChildDevice->m_vectDeviceData_Impl_Children.end(); ++it1 )
			{
				pChildDevice1 = (*it1);
				if( pChildDevice1 != NULL )
				{
					string tmp_node_id = pChildDevice1->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					if ( tmp_node_id.compare(sInternalID) == 0) {
					        return pChildDevice1;
					}
				}
			}

		}
	}
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "agocontrol_Bridge::InternalIDToDevice() No device found for id %s", sInternalID.c_str());
	return NULL;
}


int agocontrol_Bridge::AddDevice(int parent, string sInternalID,int PK_DeviceTemplate, string sName, string sRoom) {
	int iPK_Device = 0;

	int tmp_parent = 0;
	if (parent > 0) { tmp_parent = parent; } else { tmp_parent = m_dwPK_Device; }

	string tmp_s = StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalID;
	DeviceData_Impl *pDevice = InternalIDToDevice(sInternalID);
	bool bFound = pDevice != NULL;
	
	if (!bFound) {
		// does not exist, create child
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Adding device for node: %s",sInternalID.c_str());
		CMD_Create_Device add_command(m_dwPK_Device,4, PK_DeviceTemplate,"",0,"",
			StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalID, 0,tmp_parent,sName,0,0,&iPK_Device) ;
		SendCommand(add_command);
		if (iPK_Device > 0) {
			string sCMD_Result;

			CMD_Set_Room_For_Device set_room(m_dwPK_Device,4,iPK_Device,sRoom,0);
			SendCommand(set_room, &sCMD_Result);
			DCE::LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Setting room for device %i - %s, Result: %s",iPK_Device,sRoom.c_str(),sCMD_Result.c_str());
		}
	} else {
	        iPK_Device = pDevice->m_dwPK_Device;
	}

	return iPK_Device;

} 
bool agocontrol_Bridge::DeleteDevicesForNode(string sInternalID) {
	DeviceData_Impl *pTargetChildDevice = NULL;

        vector<DeviceData_Impl *> vectDevices = FindDevicesForNode(sInternalID);
	if (vectDevices.size() > 0) {
	        for( vector<DeviceData_Impl*>::const_iterator it = vectDevices.begin(); it != vectDevices.end(); ++it )
		{
		        pTargetChildDevice = (*it);
			CMD_Delete_Device del_command(m_dwPK_Device,4,pTargetChildDevice->m_dwPK_Device);
			DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Deleting DCE child %i, node uuid %s",pTargetChildDevice->m_dwPK_Device,sInternalID.c_str());

			SendCommand(del_command);
		}
		return true;
	} else {
		return false;
	}
}

vector<DeviceData_Impl*> agocontrol_Bridge::FindDevicesForNode(string sInternalID) {
        vector<DeviceData_Impl*> vecDevices;
	DeviceData_Impl* pChildDevice = NULL;

	if (sInternalID == "") return vecDevices;
        string sInternalIDInst = sInternalID;
        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
        {
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
		        string tmp_node_id = pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
			if (tmp_node_id.find("/") != string::npos) {
 	                        vector<string> parts;
 	                        StringUtils::Tokenize(tmp_node_id, "/", parts);
 	                        tmp_node_id = parts[0];
 	                } 
			// check if child exists
			if ( tmp_node_id.compare(sInternalIDInst) == 0) {
			      vecDevices.push_back(pChildDevice);
			}

			// iterate over embedded interfaces
			DeviceData_Impl *pChildDevice1 = NULL;
			for( VectDeviceData_Impl::const_iterator it1 = pChildDevice->m_vectDeviceData_Impl_Children.begin();
				it1 != pChildDevice->m_vectDeviceData_Impl_Children.end(); ++it1 )
			{
				pChildDevice1 = (*it1);
				if( pChildDevice1 != NULL )
				{
					string tmp_node_id = pChildDevice1->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					if (tmp_node_id.find("/") != string::npos) {
						vector<string> parts;
						StringUtils::Tokenize(tmp_node_id, "/", parts);
						tmp_node_id = parts[0];
					} 
					if ( tmp_node_id.compare(sInternalIDInst) == 0) {
					         vecDevices.push_back(pChildDevice1);
					}
				}
			}

		}
	}
	return vecDevices;
}

void agocontrol_Bridge::receiveFunction() {
	Variant::Map agocommand;
	Variant::Map inventoryMap;	

	qpid::messaging::Message inventorymessage;
	string inventory;

	printf("receive thread startup\n");	
	try {
		agocommand["command"] = "inventory";
		encode(agocommand, inventorymessage);

		Address responseQueue("#response-queue; {create:always, delete:always}");
		Receiver responseReceiver = agoSession.createReceiver(responseQueue);
		inventorymessage.setReplyTo(responseQueue);

		agoSender.send(inventorymessage);
		qpid::messaging::Message response = responseReceiver.fetch();
		agoSession.acknowledge();
		decode(response,inventoryMap);
		Variant::Map deviceMap;
		if (!(inventoryMap["devices"].isVoid())) deviceMap = inventoryMap["devices"].asMap();
		Variant::Map roomMap;
		if (!(inventoryMap["rooms"].isVoid())) roomMap = inventoryMap["rooms"].asMap();
		for (Variant::Map::iterator it = deviceMap.begin(); it!=deviceMap.end(); it++) {
			Variant::Map device;
			string sRoomName = "";
			if (it->second.isVoid()) continue;
			device = it->second.asMap();

			Variant::Map::iterator roomIt = roomMap.find(device["room"].asString());
			if ((roomIt != roomMap.end()) && (!(roomIt->second.isVoid()))) {
				Variant::Map room = roomIt->second.asMap();
				sRoomName = room["name"].asString();
			}
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"ago control device found: %s type: %s name: %s room: %s", it->first.c_str(),device["devicetype"].asString().c_str(),device["name"].asString().c_str(),sRoomName.c_str());

			if (device["devicetype"] == "switch") AddDevice(0, it->first.c_str(), DEVICETEMPLATE_Light_Switch_onoff_CONST,device["name"].asString(),sRoomName);
			if (device["devicetype"] == "dimmer") AddDevice(0, it->first.c_str(), DEVICETEMPLATE_Light_Switch_dimmable_CONST,device["name"].asString(),sRoomName);
			if (device["devicetype"] == "drapes") AddDevice(0, it->first.c_str(), DEVICETEMPLATE_Drapes_Switch_CONST,device["name"].asString(),sRoomName);
			if (device["devicetype"] == "binarysensor") AddDevice(0, it->first.c_str(), DEVICETEMPLATE_Generic_Sensor_CONST,device["name"].asString(),sRoomName);
			if (device["devicetype"] == "multilevelsensor") AddDevice(0, it->first.c_str(), DEVICETEMPLATE_Multilevel_Sensor_CONST,device["name"].asString(),sRoomName);
			if (device["devicetype"] == "smokedetector") AddDevice(0, it->first.c_str(), DEVICETEMPLATE_Smoke_Detector_CONST,device["name"].asString(),sRoomName);


		} 
	} catch(const std::exception& error) {
                std::cerr << error.what() << std::endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not fetch inventory from ago control");
	}

	while (true) {
		try{
			Variant::Map content;
			qpid::messaging::Message message = agoReceiver.fetch(Duration::SECOND * 3);
			agoSession.acknowledge();

			// workaround for bug qpid-3445
			if (message.getContent().size() < 4) {
				throw qpid::messaging::EncodingException("message too small");
			}

			decode(message, content);
			string subject = message.getSubject();
			if (subject!="") { // subject set, this is an event
				DeviceData_Impl *pChildDevice = NULL;
				pChildDevice = InternalIDToDevice(content["uuid"].asString());
				if (pChildDevice != NULL) {
					if (subject == "event.device.statechanged") {
						LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Device state changed: %s %s",content["uuid"].asString().c_str(),content["level"].asString().c_str());
						SendLightChangedEvent(pChildDevice->m_dwPK_Device,content["level"]);
					}
					if (subject == "event.environment.brightnesschanged") {
						LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Brightness changed: %s %s",content["uuid"].asString().c_str(),content["level"].asString().c_str());
						SendLightChangedEvent(pChildDevice->m_dwPK_Device,content["level"].asUint8());
					}
					if (subject == "event.environment.temperaturechanged") {
						LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Temperature changed: %s %s",content["uuid"].asString().c_str(),content["level"].asString().c_str());
						SendTemperatureChangedEvent(pChildDevice->m_dwPK_Device,atof(content["level"].asString().c_str()));
					}
					if (subject == "event.environment.humiditychanged") {
						LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Humidity changed: %s %s",content["uuid"].asString().c_str(),content["level"].asString().c_str());
						SendHumidityChangedEvent(pChildDevice->m_dwPK_Device,content["level"]);
					}
					if (subject == "event.environment.sensorchanged") {
						LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sensor status changed: %s %s",content["uuid"].asString().c_str(),content["level"].asString().c_str());
						SendSensorTrippedEvent(pChildDevice->m_dwPK_Device,content["level"].asUint8() == 0 ? false : true);
					}
				}
			}
		
		} catch(const NoMessageAvailable& error) {
			
		} catch(const std::exception& error) {
			std::cerr << error.what() << std::endl;
		}

	}
}



void agocontrol_Bridge::SendTemperatureChangedEvent(unsigned int PK_Device, float value)
{
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "agocontrol_Bridge::SendTemperatureChangedEvent(): PK_Device %d", PK_Device);
	char tempstr[512];
	sprintf(tempstr, "%.2f", value);
	m_pEvent->SendMessage( new Message(PK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Temperature_Changed_CONST, 1, 
			EVENTPARAMETER_Value_CONST, tempstr)
		);

}

void agocontrol_Bridge::SendSensorTrippedEvent(unsigned int PK_Device, bool value)
{
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sending sensor tripped event from PK_Device %d", PK_Device);
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Sensor_Tripped_CONST,
					   1,
					   EVENTPARAMETER_Tripped_CONST,
					   value ? "1" : "0")
		);
}

void agocontrol_Bridge::SendLightChangedEvent(unsigned int PK_Device, int value)
{
	string svalue = StringUtils::itos(value);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sending EVENT_State_Changed_CONST event from PK_Device %d, level %s",PK_Device,svalue.c_str());
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_State_Changed_CONST,
					   1,
					   EVENTPARAMETER_State_CONST,
					   svalue.c_str())
		);
}

void agocontrol_Bridge::SendPowerUsageChangedEvent(unsigned int PK_Device, int value)
{
	string svalue = StringUtils::itos(value);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sending EVENT_Power_Usage_Changed_CONST event from PK_Device %d, value %s W",PK_Device,svalue.c_str());
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Power_Usage_Changed_CONST,
					   1,
					   EVENTPARAMETER_Watts_CONST,
					   svalue.c_str())
		);
}
void agocontrol_Bridge::SendVoltageChangedEvent(unsigned int PK_Device, int value)
{
	string svalue = StringUtils::itos(value);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sending EVENT_Voltage_Changed_CONST event from PK_Device %d, value %s W",PK_Device,svalue.c_str());
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Voltage_Changed_CONST,
					   1,
					   EVENTPARAMETER_Voltage_CONST,
					   svalue.c_str())
		);
}

void agocontrol_Bridge::SendOnOffEvent(unsigned int PK_Device, int value) {
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sending EVENT_OnOff_CONST event from PK_Device %d, value %d",PK_Device, value);
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,
					   EVENT_Device_OnOff_CONST,
					   1,
					   EVENTPARAMETER_OnOff_CONST,
					   (value == 0) ? "0" : "1")
		);
}

void agocontrol_Bridge::SendBrightnessChangedEvent(unsigned int PK_Device, int value)
{
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sending brightness level changed event from PK_Device %d, value %d",PK_Device, value);
	m_pEvent->SendMessage( new Message(PK_Device,
					   DEVICEID_EVENTMANAGER,
					   PRIORITY_NORMAL,
					   MESSAGETYPE_EVENT,
					   EVENT_Brightness_Changed_CONST, 1, 
					   EVENTPARAMETER_Value_CONST, StringUtils::itos(value).c_str())
		);
}

void agocontrol_Bridge::SendFireAlarmEvent(unsigned int PK_Device)
{
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sending fire alarm event from PK_Device %d",PK_Device);
	m_pEvent->SendMessage( new Message(PK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Fire_Alarm_CONST,
			1,
			EVENTPARAMETER_PK_Device_CONST,
			PK_Device)
	);
}

void agocontrol_Bridge::SendHumidityChangedEvent(unsigned int PK_Device, float value)
{
	char tempstr[512];
	snprintf(tempstr, 512, "%.2f", value);
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Sending humidity level changed event from PK_Device %s",PK_Device);
	m_pEvent->SendMessage( new Message(PK_Device,
			DEVICEID_EVENTMANAGER,
			PRIORITY_NORMAL,
			MESSAGETYPE_EVENT,
			EVENT_Humidity_Changed_CONST, 1, 
			EVENTPARAMETER_Value_CONST, tempstr)
	);
}

