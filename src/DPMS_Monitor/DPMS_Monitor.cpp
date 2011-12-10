/*
     Copyright (C) 2009 LinuxMCE 

     www.linuxmce.com

	Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "DPMS_Monitor.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <sstream>
#include <unistd.h> // for usleep()

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
DPMS_Monitor::DPMS_Monitor(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: DPMS_Monitor_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
DPMS_Monitor::DPMS_Monitor(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: DPMS_Monitor_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
	m_pDisplay = NULL;
}

//<-dceag-dest-b->
DPMS_Monitor::~DPMS_Monitor()
//<-dceag-dest-e->
{
	
}

bool DPMS_Monitor::getOffTimeFromOrbiter(int& iOff)
{
	DeviceData_Base *pOrbiter = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Standard_Orbiter_CONST);
	if (!pOrbiter)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to find Orbiter device to grab timeout values.");
		return false;
	}
	else
	{
		string sTimeout = m_pData->m_pEvent_Impl->GetDeviceDataFromDatabase(pOrbiter->m_dwPK_Device,DEVICEDATA_Timeout_CONST);
		string sToken;
		unsigned int iTokenPos = 0;
		int iTimeScreensaver = 0;
		int iTimeOff = 0;

		if ((sToken = StringUtils::Tokenize(sTimeout,",",iTokenPos)) != "")
		{
			istringstream tmpStream;
			tmpStream.str(sToken);
			tmpStream >> iTimeScreensaver;
		}

		if ((sToken = StringUtils::Tokenize(sTimeout,",",iTokenPos)) != "")
		{
			istringstream tmpStream;
			tmpStream.str(sToken);
			tmpStream >> iTimeOff;
		}

		iOff = iTimeScreensaver + iTimeOff;

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"DPMS Timeout calculated to be %d seconds.",iOff);
	}

	return true;
}

//<-dceag-getconfig-b->
bool DPMS_Monitor::GetConfig()
{
	if( !DPMS_Monitor_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	m_pDisplay = XOpenDisplay(getenv("DISPLAY"));

	if (!m_pDisplay) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could Not Open X Display.");
		return false;
	}

	if (DPMSCapable(m_pDisplay) == False)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X Server is not DPMS Capable");
		return false;
	}

	// Explicitly set timeouts. These are gathered from the Orbiter's Timeout device data.
	// Note that standby and suspend are set to 0, so that the natural screensaver runs.
	// Off is set to the second timeout value.
	
//	if (!getOffTimeFromOrbiter(m_iOff))
//	{
//		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to get Screen Off time from nearby Orbiter.");
//		return false;
//	}

	// Finally, enable DPMS, and set timeouts.
	DPMSEnable(m_pDisplay);
	DPMSSetTimeouts(m_pDisplay, 0, 0, 0); // yeah yeah, wound up not using it. 
	XFlush(m_pDisplay);
	
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool DPMS_Monitor::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
DPMS_Monitor_Command *Create_DPMS_Monitor(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new DPMS_Monitor(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void DPMS_Monitor::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void DPMS_Monitor::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

/**
 * Thread entry point for pthread to call DPMS_Monitor::DPMSMonitorThread()
 */
void * StartDPMSMonitorThread(void * Arg)
{
  DCE::DPMS_Monitor *pDPMSMonitor = (DCE::DPMS_Monitor *) Arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pDPMSMonitor->DPMSMonitorThread();
  return NULL;
}

/**
 * Custom CreateChildren method that spawns a polling thread checking the status of the DPMS
 * and sending an appropriate CMD_Display_OffOn to the closest Orbiter. This makes sure that
 * Orbiter knows the actual state of the display, and we don't get the display staying on
 * because Orbiter THINKS it is off.
 */
void DPMS_Monitor::CreateChildren()
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"DPMS_Monitor::CreateChildren() - spawning DPMS Monitoring Thread.");
  if (pthread_create(&m_DPMSMonitorThread, NULL, StartDPMSMonitorThread, (void *) this))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DPMS_Monitor::CreateChildren() - Failed to create DPMS Monitoring Thread.");
      return;
    }
  return;
}

/**
 * Send a CMD_Display_OnOff to the closest onscreen orbiter.
 */
void DPMS_Monitor::SendOnOffToOrbiter(bool bOnOff)
{
  string sOnOff = "";
  DeviceData_Base *pDevice_Orbiter = 
    m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Standard_Orbiter_CONST,this);

  if (bOnOff)
    {
      sOnOff = "1";
    }
  else
    {
      sOnOff = "0";
    }
  
  if (pDevice_Orbiter)
    { 
      DCE::CMD_Display_OnOff CMD_Display_OnOff(m_dwPK_Device,
					       pDevice_Orbiter->m_dwPK_Device,
					       sOnOff,
					       false);
      SendCommand(CMD_Display_OnOff);
    }
  else
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DPMS_Monitor::SendOnOffToOrbiter - Could not find the onscreen orbiter. Timed out during registration.");
}

/**
 * Sits here, in a runloop, checking DPMS status and sending CMD_Display_OnOff as needed to the
 * closest related orbiter, until we need to quit.
 */
void DPMS_Monitor::DPMSMonitorThread()
{

  CARD16 currentPower, previousPower;
  BOOL state;

  while (!m_bQuit_get())
    {
      if (m_pDisplay)
	{
	  XFlush(m_pDisplay);
	  usleep(250000); // Wait just a bit for X to sync.
	  previousPower = currentPower;
	  if (!DPMSInfo(m_pDisplay,&currentPower,&state))
	    {
	      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DPMS_Monitor::DPMSMonitorThread() - Could not grab DPMS info from the display.");
	      return;
	    }
	  if (previousPower!=currentPower)
	    {
	      switch(currentPower)
		{
		case DPMSModeOn:
		  SendOnOffToOrbiter(true); // on
		  break;
		case DPMSModeOff:
		  SendOnOffToOrbiter(false); // off
		  break;
		}
	    }
	}
    }
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/**  */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void DPMS_Monitor::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
	cout << "Need to implement command #192 - On" << endl;
	cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
	cout << "Parm #98 - PK_Device_Pipes=" << sPK_Device_Pipes << endl;

	DPMSForceLevel(m_pDisplay,DPMSModeOn);
	XFlush(m_pDisplay);
}

//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/**  */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void DPMS_Monitor::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
	cout << "Need to implement command #193 - Off" << endl;
	cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;

	DPMSForceLevel(m_pDisplay,DPMSModeOff);
	XFlush(m_pDisplay);
}


