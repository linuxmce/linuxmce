/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "CEC_Adaptor.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <libcec/cecloader.h>
#include "PlutoUtils/LinuxSerialUSB.h"
using namespace CEC;

libcec_configuration g_config;
ICECCallbacks g_callbacks;

/**
 * Globals for libCEC - find a better way to do this.
 */

int CecLogMessage(void *UNUSED(cbParam), const cec_log_message &message)
{
  switch (message.level)
    {
    case CEC_LOG_ERROR:
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"libCEC: %s",message.message);
      break;
    case CEC_LOG_WARNING:
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"libCEC: %s",message.message);
      break;
    case CEC_LOG_NOTICE:
    case CEC_LOG_TRAFFIC:
    case CEC_LOG_DEBUG:
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"libCEC: %s",message.message);
      break;
    default:
      break;
    }
  return 0;
}

int CecKeyPress(void *UNUSED(cbParam), const cec_keypress &UNUSED(key))
{
  // TODO
  return 0;
}

int CecCommand(void *UNUSED(cbParam), const cec_command &UNUSED(command))
{
  // TODO
  return 0;
}


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
CEC_Adaptor::CEC_Adaptor(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: CEC_Adaptor_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
CEC_Adaptor::CEC_Adaptor(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: CEC_Adaptor_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
CEC_Adaptor::~CEC_Adaptor()
//<-dceag-dest-e->
{
	
}

void CEC_Adaptor::PrepareToDelete()
{
  Command_Impl::PrepareToDelete();
  if (m_pParser)
    {
      m_pParser->Close();
      UnloadLibCec(m_pParser);
      m_pParser=NULL;
    }
}

//<-dceag-getconfig-b->
bool CEC_Adaptor::GetConfig()
{
	if( !CEC_Adaptor_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	g_config.Clear();
	snprintf(g_config.strDeviceName, 13, "LinuxMCE");
	g_config.callbackParam = NULL;
	g_config.clientVersion = CEC_CLIENT_VERSION_1_6_1;
	g_callbacks.CBCecLogMessage = &CecLogMessage;
	g_callbacks.CBCecKeyPress = &CecKeyPress;
	g_callbacks.CBCecCommand = &CecCommand;
	g_config.callbacks = &g_callbacks;

	g_config.deviceTypes.add(CEC_DEVICE_TYPE_RECORDING_DEVICE);

	m_pParser = LibCecInitialise(&g_config);

	if (m_pParser)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not load libcec.so. Bailing!");
	    return false;
	  }

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"libCEC initialized - Version %s",m_pParser->ToString((cec_server_version)g_config.serverVersion));

	m_sPort = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());

	if (m_sPort.empty())
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No COM Port specified. Please set one in Interfaces section of Web Admin, or re-detect device.");
	    return false;
	  }

	if (!m_pParser->Open(m_sPort.c_str()))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not open serial port %s",m_sPort.c_str());
	  }

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool CEC_Adaptor::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
CEC_Adaptor_Command *Create_CEC_Adaptor(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new CEC_Adaptor(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void CEC_Adaptor::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void CEC_Adaptor::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void CEC_Adaptor::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	cout << "Need to implement command #191 - Send Code" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
}

//<-dceag-c245-b->

	/** @brief COMMAND: #245 - Learn IR */
	/** The next IR code received is to be learned in Pronto format and fire a Store IR Code command to the I/R Plugin when done */
		/** @param #2 PK_Device */
			/** You can specify the device to learn for here, or you can send the command to the device itself and leave this blank */
		/** @param #8 On/Off */
			/** Turn IR Learning mode on or off
0, 1 */
		/** @param #25 PK_Text */
			/** If specified, the text object  which should contain the result of the learn command */
		/** @param #154 PK_Command */
			/** Command ID for which the learning is done for */

void CEC_Adaptor::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
	cout << "Need to implement command #245 - Learn IR" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
	cout << "Parm #25 - PK_Text=" << iPK_Text << endl;
	cout << "Parm #154 - PK_Command=" << iPK_Command << endl;
}

//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

void CEC_Adaptor::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	cout << "Need to implement command #687 - Set Screen Type" << endl;
	cout << "Parm #48 - Value=" << iValue << endl;
}


