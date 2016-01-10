/*
     Copyright (C) 2009 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "EnOcean_TCM120.h"
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
	#include "libEnOcean/cssl.h"
	#include "libEnOcean/EnOceanProtocol.h"
	#include "libEnOcean/EnOceanPort.h"
	#include "libEnOcean/TCM120.h"
}

void serialCallBack(enocean_data_structure in);

void *myself;
 
//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
EnOcean_TCM120::EnOcean_TCM120(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: EnOcean_TCM120_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
EnOcean_TCM120::EnOcean_TCM120(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: EnOcean_TCM120_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
EnOcean_TCM120::~EnOcean_TCM120()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool EnOcean_TCM120::GetConfig()
{
	if( !EnOcean_TCM120_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	myself = (void *)this;
	
	string port = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	enocean_error_structure error = enocean_init((const char *)port.c_str());
	if (error.code != E_OK) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"%s\n",error.message);
		return false;
	}
	enocean_set_callback_function(serialCallBack);

	enocean_data_structure frame;
	frame = enocean_clean_data_structure();
	frame = tcm120_reset();

	/* char* humanstring;
	humanstring = enocean_hexToHuman(frame);
	printf("frame is: %s",humanstring);
	free(humanstring); */

	enocean_send(&frame);

	// give the TCM120 time to handle the command
	sleep (1);

	frame = enocean_clean_data_structure();
	frame = tcm120_rd_idbase();

	enocean_send(&frame);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool EnOcean_TCM120::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
EnOcean_TCM120_Command *Create_EnOcean_TCM120(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new EnOcean_TCM120(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void EnOcean_TCM120::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void EnOcean_TCM120::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void EnOcean_TCM120::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
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

void EnOcean_TCM120::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
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

void EnOcean_TCM120::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
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

void EnOcean_TCM120::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
{
	cout << "Need to implement command #788 - StatusReport" << endl;
	cout << "Parm #51 - Arguments=" << sArguments << endl;
}


// internal methods

void serialCallBack(enocean_data_structure in) {
	DeviceData_Impl *pTargetChildDevice = NULL;

	long id = (in.ID_BYTE3 << 24) + (in.ID_BYTE2 << 16) + (in.ID_BYTE1 << 8) + in.ID_BYTE0;
	pTargetChildDevice = (static_cast<EnOcean_TCM120*>(myself))->InternalIDToDevice(id);
//	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Node %i Internal: %p",id,pTargetChildDevice);

	switch(in.ORG) {
		case C_ORG_INF_INIT:
			if (in.DATA_BYTE3 != 0x20) LoggerWrapper::GetInstance()->Write(LV_WARNING,"Received INF_INIT %c%c%c%c%c%c%c%c\n",in.DATA_BYTE3,in.DATA_BYTE2,in.DATA_BYTE1,in.DATA_BYTE0,in.ID_BYTE3,in.ID_BYTE2,in.ID_BYTE1,in.ID_BYTE0);
			break;
			;;
		case C_ORG_INF_IDBASE:
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Received INF_IDBASE 0x%02x%02x%02x%02x",in.ID_BYTE3,in.ID_BYTE2,in.ID_BYTE1,in.ID_BYTE0);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TCM120 Module initialized");
			break;
			;;
		case C_ORG_RPS:
			if (in.STATUS & S_RPS_NU) {
				// NU == 1, N-Message
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Received RPS N-Message Node 0x%08x Rocker ID: %i UD: %i Pressed: %i Second Rocker ID: %i SUD: %i Second Action: %i", id,
					(in.DATA_BYTE3 & DB3_RPS_NU_RID) >> DB3_RPS_NU_RID_SHIFT, 
					(in.DATA_BYTE3 & DB3_RPS_NU_UD) >> DB3_RPS_NU_UD_SHIFT, 
					(in.DATA_BYTE3 & DB3_RPS_NU_PR)>>DB3_RPS_NU_PR_SHIFT,
					(in.DATA_BYTE3 & DB3_RPS_NU_SRID)>>DB3_RPS_NU_SRID_SHIFT, 
					(in.DATA_BYTE3 & DB3_RPS_NU_SUD)>>DB3_RPS_NU_SUD_SHIFT,
					(in.DATA_BYTE3 & DB3_RPS_NU_SA)>>DB3_RPS_NU_SA_SHIFT);
				

				if (pTargetChildDevice != NULL) {
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending On/Off event from Node 0x%08x",id);
					(static_cast<EnOcean_TCM120*>(myself))->m_pEvent->SendMessage( new Message(pTargetChildDevice->m_dwPK_Device,
						DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,
						EVENT_Device_OnOff_CONST,
						1,
						EVENTPARAMETER_OnOff_CONST,
						((in.DATA_BYTE3 & DB3_RPS_NU_UD) >> DB3_RPS_NU_UD_SHIFT) ? "1" : "0")
					);
				}

				
			} else {
				// NU == 0, U-Message
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Received RPS U-Message Node 0x%08x Buttons: %i Pressed: %i",id,(in.DATA_BYTE3 & DB3_RPS_BUTTONS) >> DB3_RPS_BUTTONS_SHIFT, (in.DATA_BYTE3 & DB3_RPS_PR)>>DB3_RPS_PR_SHIFT);
			}
			break;
			;;
		case C_ORG_1BS:
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Received 1BS Message Node 0x%08x Value 0x%x",id,in.DATA_BYTE3);
			if (pTargetChildDevice != NULL) {
				if (((in.STATUS >> 4) & 7) == 0) { // magnet contact profile, send sensor tripped events
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Sending sensor tripped event from Node 0x%08x",id);
					(static_cast<EnOcean_TCM120*>(myself))->m_pEvent->SendMessage( new Message(pTargetChildDevice->m_dwPK_Device,
						DEVICEID_EVENTMANAGER,
						PRIORITY_NORMAL,
						MESSAGETYPE_EVENT,
						EVENT_Sensor_Tripped_CONST,
						1,
						EVENTPARAMETER_Tripped_CONST,
						(in.DATA_BYTE3 & 1) ? "0" : "1")
					);
				}
			}

			break;
			;;
		case C_ORG_4BS:
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Received 4BS Message Node 0x%08x",id);
			if ((in.DATA_BYTE0 & 8) == 8) {
				// teach in telegram
				if ((in.DATA_BYTE0 & 128) == 128) {
					// new type
					int profile = in.DATA_BYTE3 >> 2;
					int type = ((in.DATA_BYTE3 & 3) << 5) + (in.DATA_BYTE2 >> 3);
					int manufacturer = ((in.DATA_BYTE2 & 7) << 8) + in.DATA_BYTE1;
					switch (profile) {
						case 2:	// Temp sensor
							break;
							;;
						case 4:	// Temp & Hum sensor
							break;
							;;
						case 5:	// pressure
							break;
							;;
						case 6:	// Light sensor
							break;
							;;
						case 7:	// Occupancy
							break;
							;;
						case 8:	// Light, Temp, Occup
							break;
							;;
						case 9:	// gas sensor
							break;
							;;
						case 16:	// room operating panel
							break;
							;;
						case 48:	// digital input
							break;
							;;
						case 56:	// central command
							break;
							;;
						default:
							;;
					}
				}

			} else {
				// regular telegram


			}
			break;
			;;
		default:
			char* humanstring;
			humanstring = enocean_hexToHuman(in);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Received unhandled frame: %s",humanstring);
			printf("DATA: %s\n",humanstring);
			free(humanstring);
			break;
			;;
	}



}
DeviceData_Impl *EnOcean_TCM120::InternalIDToDevice(long lInternalID) {
        DeviceData_Impl *pChildDevice = NULL;

        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
        {
                pChildDevice = (*it);
                if( pChildDevice != NULL )
                {
			int tmp_node_id = atoi(pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str());
			// check if child exists
			if ( tmp_node_id == lInternalID) {
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
					int tmp_node_id = atoi(pChildDevice1->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str());
					if ( tmp_node_id == lInternalID) {
						return pChildDevice1;
					}
				}
			}

		}
	}
	return NULL;
}
