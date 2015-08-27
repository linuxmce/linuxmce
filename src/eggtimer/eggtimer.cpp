/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "eggtimer.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DeviceData_Router.h"
#include "pluto_main/Define_ParameterType.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
eggtimer::eggtimer(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: eggtimer_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pAlarmManager=NULL;
	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(1);
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
eggtimer::eggtimer(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: eggtimer_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
eggtimer::~eggtimer()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool eggtimer::GetConfig()
{
	if( !eggtimer_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool eggtimer::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
eggtimer_Command *Create_eggtimer(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new eggtimer(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void eggtimer::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void eggtimer::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void eggtimer::SomeFunction()
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

//<-dceag-c1146-b->

	/** @brief COMMAND: #1146 - Start Egg Timer */
	/** Start egg timer and send On command to designated device. */
		/** @param #124 DeviceToLink */
			/** Device to keep on for the amount of time the egg timer runs. */
		/** @param #182 Timeout */
			/** Seconds before the associated device receives an off command. */
		/** @param #290 VerifyStateDeviceID */
			/** Device ID to verify for state tripped upon end of timer. If tripped, the timer be extended. */

void eggtimer::CMD_Start_Egg_Timer(int iDeviceToLink,string sTimeout,int iVerifyStateDeviceID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1156-e->
{
	int nTimeout;
	int oldID;
	nTimeout = atoi(sTimeout.c_str());

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Start_Egg_Timer(): Starting timer");
	
	oldID = m_pAlarmManager->FindAlarmByType(iDeviceToLink);
	m_pAlarmManager->CancelAlarm(oldID);
	Message* pDuplicatedMessage = new Message(pMessage);
	m_pAlarmManager->AddRelativeAlarm(nTimeout,this,iDeviceToLink,(void*)pDuplicatedMessage);
	
	CommandOn(iDeviceToLink);

        int nVerifyStateDeviceID = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_VerifyStateDeviceID_CONST].c_str());
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Start_Egg_Timer(): current VerifyStateDeviceID is %d", nVerifyStateDeviceID);
        
}


//<-dceag-c1147-b->

	/** @brief COMMAND: #1147 - Cancel Egg Timer */
	/** Manually stop egg timer  */
		/** @param #124 DeviceToLink */
			/** Device ID with associated egg timer */
		/** @param #289 SendOFF */
			/** Do send the off command to the designated device. */

void eggtimer::CMD_Cancel_Egg_Timer(int iDeviceToLink,bool bSendOFF,string &sCMD_Result,Message *pMessage)
//<-dceag-c1147-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Cancel Egg Timer for Device %n", iDeviceToLink);
	// This is a manual cancelation. We honor the users wishs whether or not the device in question
	// shall receive an OFF as well.
	if (bSendOFF) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Cancel Egg Timer with SendOFF");
		AlarmCallback(iDeviceToLink,(void*)pMessage);
	} else {
	        delete pMessage;
        }
	
	m_pAlarmManager->CancelAlarm(iDeviceToLink);	

        	
}

void eggtimer::AlarmCallback(int id, void* param)
{
	int iVerifyStateDeviceID=0;
	string sVerifyStateDeviceID="";
	bool bTurnOff = true;
	string sTmp = "";

	int nTimeout;
	int oldID;
	
	oldID = m_pAlarmManager->FindAlarmByType(id);

	Message* pMessage = (Message*)param;
	sVerifyStateDeviceID = pMessage->m_mapParameters[COMMANDPARAMETER_VerifyStateDeviceID_CONST].c_str();
	iVerifyStateDeviceID = atoi(sVerifyStateDeviceID.c_str());
	nTimeout = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Timeout_CONST].c_str());
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AlarmCallback(): current VerifyStateDeviceID is %d and %s", iVerifyStateDeviceID, sVerifyStateDeviceID.c_str());

	if (iVerifyStateDeviceID > 0) 
	{

                string status="UNKNOWN";
                // 4 is General Info Plugin
                CMD_Get_Device_Status getDeviceStatus(m_dwPK_Device, 4, iVerifyStateDeviceID, &status);
                string cResponse="";
                SendCommand(getDeviceStatus, &cResponse);
                if (cResponse=="OK")
                {
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"AlarmCallback(): current State for device is %s", status.c_str());
                        if (status == "TRIPPED") 
                        {
                                m_pAlarmManager->CancelAlarm(oldID);
                                m_pAlarmManager->AddRelativeAlarm(nTimeout,this,id,(void*)pMessage);              
                                
                        }
		}
	}
         
         
 	if (bTurnOff) 
 	{
                delete pMessage;
		CommandOff(id);		
	}
}

void eggtimer::CommandOn(int PK_Device)
{
	
	DCE::CMD_On CMD_On(m_dwPK_Device,PK_Device,0,"");
	SendCommand(CMD_On);
}

void eggtimer::CommandOff(int PK_Device)
{
	DCE::CMD_Off CMD_Off(m_dwPK_Device,PK_Device,0);
	SendCommand(CMD_Off);
}

