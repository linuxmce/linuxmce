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
#include "EIB.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "telegrammessage.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "PlutoUtils/LinuxSerialUSB.h"
#include "Serial/SerialPort.h"

#define MAX_TEMPSTR_LENGTH	15
#define DEFAULT_EIB_BPS 			19200
#define DEFAULT_EIB_PARITY 			epbsN81

using namespace EIBBUS;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
EIB::EIB(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: EIB_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}
//<-dceag-getconfig-b->
bool EIB::GetConfig()
{
	if( !EIB_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	return true;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
EIB::EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: EIB_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
EIB::~EIB()
//<-dceag-dest-e->
{
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool EIB::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

bool EIB::Connect(int iPK_DeviceTemplate) {
	if(!EIB_Command::Connect(iPK_DeviceTemplate)) {
		return false;
	}

	m_msgPool.regInterceptor(this);
	
	string sPort = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	
	if(sPort.length() > 0) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using serial port: %s.", sPort.c_str());
		string bps = DATA_Get_COM_Port_BaudRate();
		int ibps = DEFAULT_EIB_BPS;
		if(!bps.empty() && bps[0] == 'B') {
			int ibps = atoi(bps.substr(1, bps.length() -1).c_str());
    		if(ibps == 0) {
        		ibps = DEFAULT_EIB_BPS;
    		}
		}
		string pbs = DATA_Get_COM_Port_ParityBitStop();
		enum eParityBitStop ipbs = DEFAULT_EIB_PARITY;
		if(!pbs.empty()) {
    		if(pbs == "N81") {
        		ipbs = epbsN81;
			} else
    		if(pbs == "E81") {
        		ipbs = epbsE81;
    		} else
    		if(pbs == "O81") {
	    		ipbs = epbsO81;
    		}
		}		
		m_msgPool.setSerialPort(sPort.c_str(),ibps,ipbs);
	}
	
	m_msgPool.Run(false);
	
/*	
	TelegramMessage tlmsg;
	tlmsg.setActionType(TelegramMessage::READ);
//	tlmsg.setActionType(TelegramMessage::WRITE);
	tlmsg.setShortUserData(1);
	tlmsg.setGroupAddress("1/0/0");
//	tlmsg.setUserData("1");
	m_msgPool.sendTelegram(&tlmsg); 
	
	TelegramMessage tlmsg1;
	while(1) {
		m_msgPool.readTelegram(&tlmsg1);
		if(tlmsg1.getActionType() == TelegramMessage::ANSWER) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Value for %s is: %d", 
			tlmsg1.getGroupAddress(), tlmsg1.getShortUserData());
		}
	}
*/	
	
	return true;
}


/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
EIB_Command *Create_EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new EIB(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void EIB::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command %d received for CHILD", pMessage->m_dwID);
    
	TelegramMessage tlmsg;
	switch(pMessage->m_dwID) {
		/***Toggle Power*********************************************************************/
		case COMMAND_Toggle_Power_CONST: {
			switch(pDeviceData_Impl->m_dwPK_DeviceTemplate) {
				case DEVICETEMPLATE_Generic_Input_Ouput_CONST: {
					string sInputOrOutput = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_InputOrOutput_CONST);
					if(sInputOrOutput == "0" || sInputOrOutput == "2") { /* check if input */
						string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
						tlmsg.setGroupAddress(sChannel.c_str());
						tlmsg.setShortUserData((unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST].c_str()));
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Turning ON");
						m_msgPool.sendTelegram(&tlmsg);
					}
				} break;
				default: {
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device type NOT Supported.");
				} break;
			}
		} break;
		
		/***On*******************************************************************************/
		case COMMAND_Generic_On_CONST: {
			/***all devices ***/
			switch(pDeviceData_Impl->m_dwPK_DeviceTemplate) {
				case DEVICETEMPLATE_Drapes_Switch_CONST:
				case DEVICETEMPLATE_Light_Switch_onoff_CONST:
				case DEVICETEMPLATE_Light_Switch_dimmable_CONST: {
					/***default***/
					vector<string> addrs;
					string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					if(getParamsFromChannel(sChannel, addrs) >= 1) {
						tlmsg.setGroupAddress(addrs[0].c_str());
						tlmsg.setShortUserData(1);
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Turning ON");
						m_msgPool.sendTelegram(&tlmsg);
					} else {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "Channel format is incorrect.");
					}
				} break;
				default: {
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device type NOT Supported.");
				} break;
			}
			
			} break;
	/***Off*****************************************************************************/
		case COMMAND_Generic_Off_CONST: {
			/***all devices ***/
			switch(pDeviceData_Impl->m_dwPK_DeviceTemplate) {
				case DEVICETEMPLATE_Drapes_Switch_CONST:
				case DEVICETEMPLATE_Light_Switch_onoff_CONST:
				case DEVICETEMPLATE_Light_Switch_dimmable_CONST: {
					/***default***/
					vector<string> addrs;
					string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					if(getParamsFromChannel(sChannel, addrs) >= 1) {
						tlmsg.setGroupAddress(addrs[0].c_str());
						tlmsg.setShortUserData(0);
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Turning OFF");
						m_msgPool.sendTelegram(&tlmsg);
					} else {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "Channel format is incorrect.");
					}
				} break;
				default: {
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device type NOT Supported.");
				} break;
			}
			} break;
		/***Set Level*************************************************************************/
		case COMMAND_Set_Level_CONST: {
			switch(pDeviceData_Impl->m_dwPK_DeviceTemplate) {
				case DEVICETEMPLATE_Light_Switch_dimmable_CONST: {
					vector<string> addrs;
					string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					if(getParamsFromChannel(sChannel, addrs) >= 2) {
						tlmsg.setGroupAddress(addrs[1].c_str());
						
						int dimmval = 
								(unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
						unsigned char dimm 
								= (255 * dimmval) / 100;
						tlmsg.setUserData(&dimm, 1);
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Dimming to %d%%.", dimmval);
						m_msgPool.sendTelegram(&tlmsg);
					} else {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "Channel format is incorrect.");
					}
				} break;
				
				case DEVICETEMPLATE_Drapes_Switch_CONST: {
					/***drapes switch***/
					vector<string> addrs;
					string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					if(getParamsFromChannel(sChannel, addrs) >= 2) {
						tlmsg.setGroupAddress(addrs[1].c_str());
						int dimmval = 
								(unsigned int)atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
						bool stepfwrd = 
								(dimmval > 0);
						unsigned nsteps = 
								(dimmval > 0) ? dimmval : -dimmval;
						if(stepfwrd) {
							LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stepping FORWARD by %d.", nsteps);
						} else {
							LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stepping BACKWORD by %d.", nsteps);
						}
						tlmsg.setShortUserData(stepfwrd);
						while(nsteps-- > 0) {
							m_msgPool.sendTelegram(&tlmsg);
							usleep(500*1000); // sleep 10 sec
						}
					} else {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "Channel format is incorrect.");
					}
				} break;
				default: {
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device type NOT Supported.");
				} break;
			}
			} break;

		/***Set Temperature*****************************************************************************/
		case COMMAND_Set_Temperature_CONST: {
			switch(pDeviceData_Impl->m_dwPK_DeviceTemplate) {
				case DEVICETEMPLATE_Standard_Thermostat_CONST: {
					vector<string> addrs;
					string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
					if(getParamsFromChannel(sChannel, addrs) >= 1) {
						tlmsg.setGroupAddress(addrs[0].c_str());
						float tempval = 
								atof(pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
						unsigned short stempr = getUShortFromFloat(tempval);
						tlmsg.setUserData((unsigned char*)&stempr, sizeof(stempr));
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting temperature to %.2f (%.2f).", getFloatFromUShort(stempr), tempval);
						m_msgPool.sendTelegram(&tlmsg);
					} else {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "Channel format is incorrect.");
					}
				} break;
				default: {
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device type NOT Supported.");
				} break;
			}
			} break;
		default:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unknown command %d received.", pMessage->m_dwID);
			return;
	}
	
	sCMD_Result = "OK";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void EIB::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

void EIB::handleUniqueTelegram(const TelegramMessage *pt) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing received Telegram..."); 
	
	VectDeviceData_Impl& vDeviceData = m_pData->m_vectDeviceData_Impl_Children;
    for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
		if(processTelegram(pt, vDeviceData[i])) {
			break;
		}
    }
}

bool EIB::processTelegram(const EIBBUS::TelegramMessage *pt, DeviceData_Impl *pDevData) {
	Message* pMessage = NULL;
	switch(pDevData->m_dwPK_DeviceTemplate) {
		case DEVICETEMPLATE_Generic_Input_Ouput_CONST: 
		case DEVICETEMPLATE_Air_Quality_Sensor_CONST: 
		case DEVICETEMPLATE_Door_Sensor_CONST: 
		case DEVICETEMPLATE_Glass_Break_Sensor_CONST: 
		case DEVICETEMPLATE_Motion_Detector_CONST: 
		case DEVICETEMPLATE_Siren_CONST: 
		case DEVICETEMPLATE_Smoke_Detector_CONST: {
			string sInputOrOutput = pDevData->mapParameters_Find(DEVICEDATA_InputOrOutput_CONST);
			if(sInputOrOutput == "0" || sInputOrOutput == "2") { /* check if output */
				string sChannel = pDevData->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
				if(sChannel == pt->getGroupAddress()) {
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sensor triggered. Sending Event."); 
					pMessage = new Message(pDevData->m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
									EVENT_Sensor_Tripped_CONST, 1, 25, StringUtils::itos(pt->getShortUserData()).c_str());
				}
			}
		} break;
		case DEVICETEMPLATE_Standard_Thermostat_CONST: {
			string sChannel = pDevData->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
			
			vector<string> addrs;
			if(getParamsFromChannel(sChannel, addrs) >= 2) {
				if(addrs[0] == pt->getGroupAddress()) {
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "SetPoint Temperature changed. Sending Event."); 
					
					unsigned short stempr;
					if(pt->getUserData((unsigned char*)&stempr, sizeof(stempr)) == sizeof(stempr)) {
						char tempstr[MAX_TEMPSTR_LENGTH];
						sprintf(tempstr, "%.1f", (float)getFloatFromUShort(stempr));
						pMessage = new Message(pDevData->m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
								EVENT_Thermostat_Set_Point_Chan_CONST, 1, 
								EVENTPARAMETER_Value_CONST, tempstr);
					} else {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "The received data length is incorrect. Possible wrong group adress..");
					}
				} else 
				if(addrs[1] == pt->getGroupAddress()) {
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Actual Temperature changed. Sending Event."); 
					
					unsigned short stempr;
					if(pt->getUserData((unsigned char*)&stempr, sizeof(stempr)) == sizeof(stempr)) {
						char tempstr[MAX_TEMPSTR_LENGTH];
						sprintf(tempstr, "%.1f", (float)getFloatFromUShort(stempr));
						pMessage = new Message(pDevData->m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
								EVENT_Temperature_Changed_CONST, 1, 
								EVENTPARAMETER_Value_CONST, tempstr);
					} else {
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "The received data length is incorrect. Possible wrong group adress..");
					}
				}
			} else {
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Channel format is incorrect for device %d.", pDevData->m_dwPK_Device);
			}
		} break;
		default: {
		} break;
	}

	if(pMessage) {
		if(!GetEvents()->SendMessage(pMessage)) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error sending Event."); 
		}
	}
		
	return false;
}

/*
bool EIB::getParamsFromChannel(const std::string& sChannel, 
							unsigned int index, std::string& param)
{
	int bindex = -1, length = 0;
	while(index  > 0) {
		bindex = sChannel.find('|', bindex + 1);
		if(bindex < 0) {
			break;
		}
		index--;
	}
	if(index > 0) {
		return false;
	}
		
	length = sChannel.find('|', bindex + 1);
	
	if(bindex < 0) {
		bindex = 0;
	}
	
	if(length < 0) {
		length = sChannel.length() - bindex;
	} else {
		length = length - bindex;
	}
	param = sChannel.substr(bindex, length);
	return true;
}
*/
int EIB::getParamsFromChannel(const std::string& sChannel, 
						std::vector<std::string>& params) {
	params.clear();
	
	int bindex = 0, length = 0;
	while(bindex >= 0) {
		length = sChannel.find('|', bindex);
		if(length < 0) {
			params.push_back(sChannel.substr(bindex, sChannel.length() - bindex));
			break;
		} else {
			params.push_back(sChannel.substr(bindex, length - bindex));
		}
		bindex = length + 1;
	}
	return params.size();
}

float EIB::getFloatFromUShort(unsigned short tempr) {
	// change bytes
	unsigned short byte0 = tempr & 0x00ff, byte1 = (tempr & 0xff00 ) >> 8;
	tempr = (byte0 << 8)+ byte1;
	
	int sign = ((tempr & 0x8000) != 0) ? 1 : 0;  // last bit
	int exponent = (tempr & 0x7800) >> 11;
	
	int pow = 1;
	for( int i = 0; i < exponent; i++ )
		pow *= 2;
	
	int mantissa = tempr & 0x07ff;
	if( sign == 1 ) // two's complement
	{
		mantissa ^= 0x7ff;
		mantissa += 1;
		mantissa &= 0x07ff;
	}
	
	int result = pow * mantissa;
	if( sign == 1 ) {
		result = (-result);
	}
	
	return (1.0) * result / 100;
}

unsigned short EIB::getUShortFromFloat( float tempr ) {
	int sign = 0;
	if(tempr < 0) {
		sign = 1;
		tempr = -tempr;
	}
	
	/*calculate mantisa*/
	float fmantissa = tempr * 100;
	int exponent = 0;
	while(fmantissa > 2047) {
		fmantissa /=  2;
		exponent += 1;
	}
	int mantissa = (int)fmantissa;
	if( sign ) { // two's complement
		mantissa -= 1;
		mantissa ^= 0x7ff;
	}
	
	unsigned short ret = mantissa + (exponent << 11) + (sign << 15);
	
	// change bytes
	unsigned short byte0 = ret & 0x00ff, byte1 = (ret & 0xff00 ) >> 8;
	return ((byte0 << 8) + byte1);
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void EIB::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/



//<-dceag-c273-b->

	/** @brief COMMAND: #273 - EIB_Write */
	/** Peforms a WRITE request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */
		/** @param #109 Data String */
			/** EIB Data */
		/** @param #110 DataType */
			/** EIB Data Type:
[1]-switch (1b)
[2]-dimming-control (4b)
[3]-time (3B)
[4]-date (4B)
[5]-value (2B)
[6]-scaling (1B)
[7]-drive control
[9]-float
[10]-16bit counter
[11]-32bit counter
[13]-ASCII character
[14]-8bit counter
[15]-character string */

void EIB::CMD_EIB_Write(string sAddress,string sData_String,int iDataType,string &sCMD_Result,Message *pMessage)
//<-dceag-c273-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received a Write request: (To= %s, Data Type=%d, Data=%s) ", 
												sAddress.c_str(), iDataType, sData_String.c_str());
	
	if(sAddress.length() == 0) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Empty Group Address passed as parameter");
		return;
	}
												
	TelegramMessage tlmsg;
	tlmsg.setGroupAddress(sAddress.c_str());
	
	switch(iDataType) {
	case DT_SWITCH:
	case DT_DIMMING_CONTROL:
	case DT_DRIVE_CONTROL:
	case 8:
		tlmsg.setShortUserData(atoi(sData_String.c_str()));
		break;
		
	case DT_SCALING:
	case DT_ASCIICHAR:
	case DT_COUNTER8: {
		unsigned char data = atoi(sData_String.c_str());
		tlmsg.setUserData(&data, 1);
		} break;
		
	case DT_VALUE:
	case DT_COUNTER16: {
		unsigned short data = atoi(sData_String.c_str());
		tlmsg.setUserData((unsigned char*)&data, 2);
		} break;
	
	case DT_TIME:
	case DT_DATE: {
		unsigned int data = atoi(sData_String.c_str());
		tlmsg.setUserData((unsigned char*)&data, 3);
		} break;
	
	case DT_FLOAT:
	case DT_COUNTER32:
	case 12: {
		unsigned int data = atoi(sData_String.c_str());
		tlmsg.setUserData((unsigned char*)&data, 4);
		} break;
	
	case DT_STRING: {
		tlmsg.setUserData((unsigned char*)sData_String.c_str(), sData_String.length());
		} break;
	
	default:
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unknown TYPE specified for Write request");
		return;
	}
	
	m_msgPool.sendTelegram(&tlmsg);
}
//<-dceag-c275-b->

	/** @brief COMMAND: #275 - EIB_Read */
	/** Performs a READ request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */

void EIB::CMD_EIB_Read(string sAddress,string &sCMD_Result,Message *pMessage)
//<-dceag-c275-e->
{
}
//<-dceag-c350-b->

	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

void EIB::CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage)
//<-dceag-c350-e->
{
}
//<-dceag-c351-b->

	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

void EIB::CMD_Process_IDLE(string &sCMD_Result,Message *pMessage)
//<-dceag-c351-e->
{
}
//<-dceag-c355-b->

	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

void EIB::CMD_Process_Initialize(string &sCMD_Result,Message *pMessage)
//<-dceag-c355-e->
{
}
//<-dceag-c356-b->

	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

void EIB::CMD_Process_Release(string &sCMD_Result,Message *pMessage)
//<-dceag-c356-e->
{
}
//<-dceag-c373-b->

	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

void EIB::CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage)
//<-dceag-c373-e->
{
}
//<-dceag-c384-b->

	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

void EIB::CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage)
//<-dceag-c384-e->
{
}
