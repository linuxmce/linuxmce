/*
     Copyright (C) 2011 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "OneWire.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

extern "C" {
	#include <owcapi.h>
	void *start( void* );
}
void *start( void *p ) {
	OneWire *base = static_cast<OneWire*>(p);
	base->receiveFunction();
	return NULL;
}

void *myself;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
OneWire::OneWire(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: OneWire_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
OneWire::OneWire(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: OneWire_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
OneWire::~OneWire()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool OneWire::GetConfig()
{
	if( !OneWire_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	myself = (void *)this;

	string port = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	if (port == "") port = "usb";

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Opening port: %s", port.c_str());
	if (!port.length()) {
		return false;
	}
	if (OW_init(port.c_str())==0) {
		static pthread_t readThread;
		pthread_create(&readThread, NULL, start, (void*)this);

		return true;
	} else {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OW_init error: %i, %s", errno, strerror(errno));
		return false;
	}
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool OneWire::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
OneWire_Command *Create_OneWire(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new OneWire(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void OneWire::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void OneWire::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c756-b->

	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void OneWire::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
	cout << "Need to implement command #756 - Report Child Devices" << endl;
}

//<-dceag-c760-b->

	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

void OneWire::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
//<-dceag-c760-e->
{
	cout << "Need to implement command #760 - Send Command To Child" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #154 - PK_Command=" << iPK_Command << endl;
	cout << "Parm #202 - Parameters=" << sParameters << endl;
}

//<-dceag-c776-b->

	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

void OneWire::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c776-e->
{
	cout << "Need to implement command #776 - Reset" << endl;
	cout << "Parm #51 - Arguments=" << sArguments << endl;
}

//<-dceag-c788-b->

	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

void OneWire::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
{
	cout << "Need to implement command #788 - StatusReport" << endl;
	cout << "Parm #51 - Arguments=" << sArguments << endl;
}

DeviceData_Impl *OneWire::InternalIDToDevice(string sInternalID) {
        DeviceData_Impl *pChildDevice = NULL;

        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
        {
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
			if (pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).compare(sInternalID) == 0) {
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
					if (pChildDevice1->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).compare(sInternalID) == 0) {
						return pChildDevice1;
					}
				}
			}

		}
	}
	return NULL;
}

void OneWire::receiveFunction() {
	while(1) {
		readDevices();
		sleep(30);
	}
}
void OneWire::readDevices() {
	string tmpstr;
	vector<string> OW_directories;
	vector<string>::const_iterator OW_dir_it;
	char * buf;
	size_t s ;
	float temp;

	if (OW_get("/",&buf,&s) != -1) {
		tmpstr.append(buf);	
		free(buf);
		StringUtils::Tokenize(tmpstr, ",", OW_directories);
		for (OW_dir_it=OW_directories.begin(); OW_dir_it != OW_directories.end() ; OW_dir_it++) {
			// get temperature sensors
			if ((OW_dir_it->find("28.")!=string::npos)||(OW_dir_it->find("10.")!=string::npos)) {
				char tmp[1024];
				char tmpbuf[1024];
				string id;
				std::map<string,float>::iterator mapIt;
				std::map<string,int>::iterator createdMapIt;
				bool bSendEvent = false;

				id = OW_dir_it->substr(0,OW_dir_it->size()-2);
				sprintf(tmp,"/%stemperature",OW_dir_it->c_str());
				// OW_get("/28.B37956020000/temperature",&buf,&s) ;
				if (OW_get(tmp,&buf,&s)!= -1) {
					sscanf(buf,"%f",&temp);
					free(buf);
					// strip whitespace
					sprintf(tmpbuf, "%.2f",temp);
					LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Temperature for %s: %.2f",id.c_str(),temp);
				
					mapIt = sensorValueMap.find(id);
					if (mapIt != sensorValueMap.end() ) {
						// found sensor
						LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Old temperature for %s: %.2f",id.c_str(),mapIt->second);
						if (mapIt->second != temp) {
							// new temp value, need to send event
							bSendEvent = true;
							// store new value in map
							sensorValueMap.erase(mapIt);
							sensorValueMap.insert(pair<string, float>(id,temp));

						} else {
							bSendEvent = false;
						}
					} else {
						// no value in map, first reading	
						sensorValueMap.insert(pair<string, float>(id,temp));
						bSendEvent = true;
					}
					if (bSendEvent) {
						DeviceData_Impl *pChildDevice = InternalIDToDevice(id);
						if (pChildDevice != NULL) {		

						(static_cast<OneWire*>(myself))->m_pEvent->SendMessage( new Message(pChildDevice->m_dwPK_Device,
							DEVICEID_EVENTMANAGER,
							PRIORITY_NORMAL,
							MESSAGETYPE_EVENT,
							EVENT_Temperature_Changed_CONST, 1, 
							EVENTPARAMETER_Value_CONST, tmpbuf)
						);
						} else {
							createdMapIt = alreadyCreatedMap.find(id);
							if (createdMapIt == alreadyCreatedMap.end() ) {
								// child not found and not yet created, let's ask the GI plugin to create one
								LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No Child found for %s, trying to create",id.c_str());
								int iPK_Device = 0;
								CMD_Create_Device add_command(m_dwPK_Device,4, 1946,"",0,"", StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + id, 0,m_dwPK_Device,"",0,0,&iPK_Device) ;
								SendCommand(add_command);
								if (iPK_Device != 0) {
									LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Child %i created",iPK_Device);
								}
								alreadyCreatedMap.insert(pair<string, int>(id,iPK_Device));
							}

						}
					}
				}
			} else {
				// directory
				// LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Directory: %s",OW_dir_it->c_str());
			}
		}
	} else {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OW_get error: %i, %s", errno, strerror(errno));
		// got an error, let's enable debug
		OW_set_error_level("5");
		OW_set_error_print("2");

	}
}
